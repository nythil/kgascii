// This file is part of KG::Ascii.
//
// Copyright (C) 2011 Robert Konklewski <nythil@gmail.com>
//
// KG::Ascii is free software; you can redistribute it and/or modify 
// it under the terms of the GNU Lesser General Public License as published by 
// the Free Software Foundation; either version 3 of the License, or 
// (at your option) any later version.
//
// KG::Ascii is distributed in the hope that it will be useful, 
// but WITHOUT ANY WARRANTY; without even the implied warranty of 
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the 
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License 
// along with KG::Ascii. If not, see <http://www.gnu.org/licenses/>.

#include "video_player.hpp"
#include <iostream>
#include <stdexcept>
#include <boost/thread/thread.hpp>
#include <boost/thread/xtime.hpp>

VideoPlayer::VideoPlayer()
    :loaded_(false)
    ,playing_(false)
    ,canDropFrames_(true)
    ,canWaitForFrame_(true)
    ,frameWidth_(0)
    ,frameHeight_(0)
    ,frameCount_(0)
    ,startFrameNo_(0)
    ,startFrameTime_(0)
    ,currentFrameNo_(0)
    ,currentFrameTime_(0)
    ,seekedFrameNo_(-1)
    ,seekedFrameTime_(-1)
    ,timerOffset_(0)
    ,lastDroppedFrames_(0)
    ,allReadFrames_(0)
    ,readFrames_(0)
{
}

VideoPlayer::~VideoPlayer()
{
}

bool VideoPlayer::load(const std::string& filename)
{
    if (!video_.open(filename))
        return false;

    loaded_ = false;

    if (!video_.grab())
        throw std::runtime_error("problem reading video");

    frameCount_ = static_cast<unsigned>(video_.get(CV_CAP_PROP_FRAME_COUNT));
    frameWidth_ = static_cast<unsigned>(video_.get(CV_CAP_PROP_FRAME_WIDTH));
    frameHeight_ = static_cast<unsigned>(video_.get(CV_CAP_PROP_FRAME_HEIGHT));

    currentFrameNo_ = static_cast<unsigned>(video_.get(CV_CAP_PROP_POS_FRAMES));
    startFrameNo_ = currentFrameNo_;
    currentFrameTime_ = video_.get(CV_CAP_PROP_POS_MSEC) / 1000.0;
    startFrameTime_ = currentFrameTime_;
    timerOffset_ = currentFrameTime_;

    allReadFrames_ = 1;
    readFrames_ = 1;

    loaded_ = true;

    onLoaded();

    return true;
}

bool VideoPlayer::isLoaded() const
{
    return loaded_;
}

bool VideoPlayer::isPlaying() const
{
    return playing_;
}

bool VideoPlayer::play()
{
    if (!loaded_)
        throw std::logic_error("no video loaded");
    if (playing_)
        throw std::logic_error("video already playing");

    playing_ = true;
    onPlaybackStart();

    bool stream_ended = false;

    positionVideo();

    timer_.restart();

    double time_left;
    while (playing_) {
        currentTime_ = timer_.elapsed() + timerOffset_;
        time_left = std::max(currentFrameTime_ - currentTime_, 0.0);

        if (!onBeforeReadFrame(time_left)) {
            stop();
            break;
        }

        cv::Mat frame;
        video_.retrieve(frame);

        currentTime_ = timer_.elapsed() + timerOffset_;
        time_left = std::max(currentFrameTime_ - currentTime_, 0.0);

        onFrameRead(frame, time_left);
        if (!playing_)
            break;

        currentTime_ = timer_.elapsed() + timerOffset_;
        time_left = std::max(currentFrameTime_ - currentTime_, 0.0);

        while (time_left > 0 && canWaitForFrame_) {
            double dtime_sec = 0.0;
            double dtime_frac = modf(time_left, &dtime_sec);
            boost::xtime xt;
            boost::xtime_get(&xt, boost::TIME_UTC);
            xt.sec += static_cast<int>(dtime_sec);
            xt.nsec += static_cast<int>(dtime_frac * 1000000000);
            boost::thread::sleep(xt);

            currentTime_ = timer_.elapsed() + timerOffset_;
            time_left = std::max(currentFrameTime_ - currentTime_, 0.0);
        }

        onFrameDisplay(frame);
        if (!playing_)
            break;

        currentTime_ = timer_.elapsed() + timerOffset_;

        bool seeked = positionVideo();

        if (currentFrameNo_ + 1 == frameCount_) {
            stream_ended = true;
            stop();
            break;
        }

        if (!seeked) {
            unsigned grab_cnt = 0;
            do {
                if (!video_.grab())
                    throw std::runtime_error("problem reading video");
                allReadFrames_++;
                grab_cnt++;
                currentFrameNo_ = static_cast<unsigned>(video_.get(CV_CAP_PROP_POS_FRAMES));
                currentFrameTime_ = video_.get(CV_CAP_PROP_POS_MSEC) / 1000.0;
            } while (currentFrameTime_ < currentTime_ && canDropFrames_);

            lastDroppedFrames_ = grab_cnt - 1;
            readFrames_++;
        }
    }

    onPlaybackEnd();

    return stream_ended;
}

void VideoPlayer::stop()
{
    if (!loaded_)
        throw std::logic_error("no video loaded");
    if (!playing_)
        throw std::logic_error("video not playing");

    playing_ = false;
}

void VideoPlayer::seekToFrame(unsigned frm_no)
{
    if (!loaded_)
        throw std::logic_error("no video loaded");

    seekedFrameNo_ = frm_no;
}

void VideoPlayer::seekToTime(double frm_tm)
{
    if (!loaded_)
        throw std::logic_error("no video loaded");

    seekedFrameNo_ = frm_tm;
}

bool VideoPlayer::canDropFrames() const
{
    return canDropFrames_;
}

bool VideoPlayer::canWaitForFrame() const
{
    return canWaitForFrame_;
}

void VideoPlayer::setCanDropFrames(bool val)
{
    canDropFrames_ = val;
}

void VideoPlayer::setCanWaitForFrame(bool val)
{
    canWaitForFrame_ = val;
}

unsigned VideoPlayer::frameWidth() const
{
    return frameWidth_;
}

unsigned VideoPlayer::frameHeight() const
{
    return frameHeight_;
}

unsigned VideoPlayer::frameCount() const
{
    return frameCount_;
}

unsigned VideoPlayer::startFrameNo() const
{
    return startFrameNo_;
}

double VideoPlayer::startFrameTime() const
{
    return startFrameTime_;
}

unsigned VideoPlayer::currentFrameNo() const
{
    return currentFrameNo_;
}

double VideoPlayer::currentFrameTime() const
{
    return currentFrameTime_;
}

double VideoPlayer::startTime() const
{
    return timerOffset_;
}

double VideoPlayer::currentTime() const
{
    return currentTime_;
}

unsigned VideoPlayer::lastDroppedFrames() const
{
    return lastDroppedFrames_;
}

unsigned VideoPlayer::allReadFrames() const
{
    return allReadFrames_;
}

unsigned VideoPlayer::readFrames() const
{
    return readFrames_;
}

void VideoPlayer::onLoaded()
{
}

void VideoPlayer::onPlaybackStart()
{
}

void VideoPlayer::onPlaybackEnd()
{
}

bool VideoPlayer::onBeforeReadFrame(double tm_left)
{
    return true;
}

void VideoPlayer::onFrameRead(cv::Mat frm, double tm_left)
{
}

void VideoPlayer::onFrameDisplay(cv::Mat frm)
{
}

bool VideoPlayer::positionVideo()
{
    if (!loaded_)
        throw std::logic_error("no video loaded");

    if (seekedFrameNo_ >= 0) {
        if (!video_.set(CV_CAP_PROP_POS_FRAMES, seekedFrameNo_))
            throw std::runtime_error("problem seeking in video");
        seekedFrameNo_ = -1;
    } else if (seekedFrameTime_ >= 0) {
        if (!video_.set(CV_CAP_PROP_POS_MSEC, seekedFrameTime_ * 1000.0))
            throw std::runtime_error("problem seeking in video");
        seekedFrameTime_ = -1;
    } else {
        return false;
    }
    if (!video_.grab())
        throw std::runtime_error("problem reading video");
    allReadFrames_++;
    readFrames_++;

    currentFrameNo_ = static_cast<unsigned>(video_.get(CV_CAP_PROP_POS_FRAMES));
    startFrameNo_ = currentFrameNo_;
    currentFrameTime_ = video_.get(CV_CAP_PROP_POS_MSEC) / 1000.0;
    startFrameTime_ = currentFrameTime_;
    timerOffset_ = currentFrameTime_;
    return true;
}
