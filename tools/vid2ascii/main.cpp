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
#include <limits>
#include <boost/optional.hpp>
#include <boost/gil/gil_all.hpp>
#include <boost/timer.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <common/cmdlinetool.hpp>
#include <common/validateoptional.hpp>
#include <kgascii/fontimage.hpp>
#include <kgascii/textsurface.hpp>
#include <kgascii/glyphmatcher.hpp>
#include <kgascii/asciifier.hpp>

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
    boost::optional<int> startFrame_;
    boost::optional<int> endFrame_;
    boost::optional<int> maxFrames_;
    boost::optional<double> startTime_;
    boost::optional<double> endTime_;
    boost::optional<double> maxTime_;
    std::string fontFile_;
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
        ("font-file,f", value(&fontFile_), "input video file")
    ;
    posDesc_.add("input-file", 1);
}

bool VideoToAscii::processArgs()
{
    requireOption("input-file");
    requireOption("font-file");
    
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
    using namespace boost::gil;

    cv::VideoCapture capture(inputFile_);
    if (!capture.isOpened())
        return -1;

    KG::Ascii::FontImage font;
    if (!font.load(fontFile_))
        return -1;
    int char_width = font.glyphWidth();
    int char_height = font.glyphHeight();

    int frame_width = static_cast<int>(capture.get(CV_CAP_PROP_FRAME_WIDTH));
    int frame_height = static_cast<int>(capture.get(CV_CAP_PROP_FRAME_HEIGHT));
    cout << "video width " << frame_width << "\n";
    cout << "video height " << frame_height << "\n";
    cout << "video frame count " << capture.get(CV_CAP_PROP_FRAME_COUNT) << "\n";
    cout << "video fps " << capture.get(CV_CAP_PROP_FPS) << "\n";

    int col_count = (frame_width + char_width - 1) / char_width;
    int row_count = (frame_height + char_height - 1) / char_height;
    cout << "output columns " << col_count << "\n";
    cout << "output rows " << row_count << "\n";

    if (startFrame_) {
        capture.set(CV_CAP_PROP_POS_FRAMES, *startFrame_);
    } else if (startTime_) {
        capture.set(CV_CAP_PROP_POS_MSEC, *startTime_ * 1000.0);
    }
    if (!capture.grab())
        return 0;

    startFrame_ = static_cast<int>(capture.get(CV_CAP_PROP_POS_FRAMES));
    startTime_ = capture.get(CV_CAP_PROP_POS_MSEC) / 1000.0;

    KG::Ascii::TextSurface text(row_count, col_count);
    KG::Ascii::GlyphMatcher matcher(font);
    KG::Ascii::Asciifier asciifier(matcher);

    boost::timer timer;

    while (true) {
        int current_frame = static_cast<int>(capture.get(CV_CAP_PROP_POS_FRAMES));
        if (maxFrames_ && (current_frame - *startFrame_) >= *maxFrames_)
            break;
        if (endFrame_ && current_frame >= *endFrame_)
            break;

        double current_time = capture.get(CV_CAP_PROP_POS_MSEC) / 1000.0;
        if (endTime_ && current_time >= *endTime_)
            break;
        if (maxTime_ && (current_time - *startTime_) >= *maxTime_)
            break;

        cv::Mat capture_frame;
        if (!capture.retrieve(capture_frame))
            break;

        cv::Mat gray_frame;
        cv::cvtColor(capture_frame, gray_frame, CV_BGR2GRAY);

        assert(gray_frame.dims == 2);
        assert(gray_frame.cols == frame_width);
        assert(gray_frame.rows == frame_height);
        assert(gray_frame.type() == CV_8UC1);

        gray8c_view_t gray_view = interleaved_view(frame_width, frame_height, 
                reinterpret_cast<gray8c_ptr_t>(gray_frame.data), 
                gray_frame.step[0]);

        text.clear();
        asciifier.generate(gray_view, text);

        for (int r = 0; r < text.rows(); ++r) {
            cout.write(text.row(r), text.cols());
            cout << "\n";
        }

        if (!capture.grab())
            break;
    }

    double total_time = timer.elapsed();

    endFrame_ = static_cast<int>(capture.get(CV_CAP_PROP_POS_FRAMES));
    endTime_ = capture.get(CV_CAP_PROP_POS_MSEC) / 1000.0;

    cout << "total frames " << (*endFrame_ - *startFrame_) << "\n";
    cout << "total video time " << (*endTime_ - *startTime_) << "\n";
    cout << "processing time " << total_time << "\n";
    cout << "processing time / frame " << total_time / (*endFrame_ - *startFrame_) << "\n";

    return 0;
}

