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

#ifndef KGASCII_TOOLS_COMMON_VIDEO_PLAYER_HPP
#define KGASCII_TOOLS_COMMON_VIDEO_PLAYER_HPP

#include <boost/noncopyable.hpp>
#include <boost/timer.hpp>
#include <opencv2/highgui/highgui.hpp>

namespace KG { namespace Ascii {
} } // namespace KG::Ascii


class VideoPlayer: boost::noncopyable
{
public:
    VideoPlayer();

    virtual ~VideoPlayer();

    bool load(const std::string& filename);

    bool isLoaded() const;

    bool isPlaying() const;

    bool play();

    void stop();

    void seekToFrame(unsigned frm_no);

    void seekToTime(double frm_tm);

    bool canDropFrames() const;

    bool canWaitForFrame() const;

    void setCanDropFrames(bool val);

    void setCanWaitForFrame(bool val);

public:
    unsigned frameWidth() const;

    unsigned frameHeight() const;

    unsigned frameCount() const;

    unsigned startFrameNo() const;

    double startFrameTime() const;

    unsigned currentFrameNo() const;

    double currentFrameTime() const;

    double startTime() const;

    double currentTime() const;

    unsigned lastDroppedFrames() const;

    unsigned allReadFrames() const;

    unsigned readFrames() const;

protected:
    virtual void onLoaded();

    virtual void onPlaybackStart();

    virtual void onPlaybackEnd();

    virtual bool onBeforeReadFrame(double tm_left);

    virtual void onFrameRead(cv::Mat frm, double tm_left);

    virtual void onFrameDisplay(cv::Mat frm);

private:
    cv::VideoCapture video_;
    bool loaded_;
    bool playing_;
    bool canDropFrames_;
    bool canWaitForFrame_;
    unsigned frameWidth_;
    unsigned frameHeight_;
    unsigned frameCount_;
    unsigned startFrameNo_;
    double startFrameTime_;
    unsigned currentFrameNo_;
    double currentFrameTime_;
    double currentTime_;
    boost::timer timer_;
    double timerOffset_;
    unsigned lastDroppedFrames_;
    unsigned allReadFrames_;
    unsigned readFrames_;
};

#endif // KGASCII_TOOLS_COMMON_VIDEO_PLAYER_HPP
