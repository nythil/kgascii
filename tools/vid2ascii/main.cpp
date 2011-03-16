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

#include <iostream>
#include <boost/optional.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <common/cmdlinetool.hpp>
#include <common/validateoptional.hpp>

using std::cout;

class VideoToAscii: public CmdlineTool
{
public:
    VideoToAscii();

protected:
    bool processArgs();

    int doExecute();
    
private:
    std::string inputFile_;
    std::string fontFile_;
    boost::optional<int> startFrame_;
    boost::optional<int> endFrame_;
    boost::optional<int> maxFrames_;
    boost::optional<double> startTime_;
    boost::optional<double> endTime_;
    boost::optional<double> maxTime_;
};


int main(int argc, char* argv[])
{
    return VideoToAscii().execute(argc, argv);
}


VideoToAscii::VideoToAscii()
    :CmdlineTool("Options")
{
    using namespace boost::program_options;
    desc_.add_options()
        ("input-file,i", value(&inputFile_), "input video file")
        ("start-frame", value(&startFrame_), "starting video frame number")
        ("end-frame", value(&endFrame_), "final video frame number")
        ("max-frames", value(&maxFrames_), "max number of frames")
        ("start-time", value(&startTime_), "starting video position in seconds")
        ("end-time", value(&endTime_), "final video position in seconds")
        ("max-time", value(&maxTime_), "max video time")
    ;
    posDesc_.add("input-file", 1);
}

bool VideoToAscii::processArgs()
{
    requireOption("input-file");
    
    conflictingOptions("start-frame", "start-time");
    conflictingOptions("start-frame", "end-time");
    conflictingOptions("start-time", "end-frame");

    conflictingOptions("end-time", "max-time");
    conflictingOptions("end-time", "max-frames");
    conflictingOptions("end-frame", "max-frames");
    conflictingOptions("end-frame", "max-time");

    if (startTime_ && endTime_ && *startTime_ > *endTime_)
        throw std::logic_error("invalid time range");
    if (startFrame_ && endFrame_ && *startFrame_ > *endFrame_)
        throw std::logic_error("invalid frame number range");

    return true;
}

int VideoToAscii::doExecute()
{
    cv::VideoCapture capture(inputFile_);
    if (!capture.isOpened())
        return -1;

    cout << "video width " << capture.get(CV_CAP_PROP_FRAME_WIDTH) << "\n";
    cout << "video height " << capture.get(CV_CAP_PROP_FRAME_HEIGHT) << "\n";
    cout << "video frame count " << capture.get(CV_CAP_PROP_FRAME_COUNT) << "\n";
    cout << "video fps " << capture.get(CV_CAP_PROP_FPS) << "\n";

    if (startFrame_) {
        capture.set(CV_CAP_PROP_POS_FRAMES, *startFrame_);
    } else if (startTime_) {
        capture.set(CV_CAP_PROP_POS_MSEC, *startTime_ * 1000.0);
    }
    if (!capture.grab())
        return 0;

    startFrame_ = static_cast<int>(capture.get(CV_CAP_PROP_POS_FRAMES));
    startTime_ = capture.get(CV_CAP_PROP_POS_MSEC) / 1000.0;

    while (true) {
        int currentFrame = static_cast<int>(capture.get(CV_CAP_PROP_POS_FRAMES));
        if (maxFrames_ && (currentFrame - *startFrame_) >= *maxFrames_)
            break;
        if (endFrame_ && currentFrame >= *endFrame_)
            break;

        double currentTime = capture.get(CV_CAP_PROP_POS_MSEC) / 1000.0;
        if (endTime_ && currentTime >= *endTime_)
            break;
        if (maxTime_ && (currentTime - *startTime_) >= *maxTime_)
            break;

        cv::Mat captureFrame;
        if (!capture.retrieve(captureFrame))
            break;

        cv::Mat grayFrame;
        cv::cvtColor(captureFrame, grayFrame, CV_BGR2GRAY);

        if (!capture.grab())
            break;
    }

    return 0;
}

