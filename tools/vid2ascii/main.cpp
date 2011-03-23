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
#include <cmath>
#include <boost/optional.hpp>
#include <boost/timer.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/xtime.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <common/cmdline_tool.hpp>
#include <common/validate_optional.hpp>
#include <common/console.hpp>
#include <kgascii/font_image.hpp>
#include <kgascii/glyph_matcher.hpp>
#include <kgascii/dynamic_asciifier.hpp>
#include <kgascii/policy_based_glyph_matcher.hpp>
#include <kgascii/squared_euclidean_distance.hpp>
#include <kgascii/means_distance.hpp>
#include <kgascii/pca_glyph_matcher.hpp>
#include <kgascii/font_pcanalyzer.hpp>
#include <kgascii/font_pca.hpp>
#include <kgascii/text_surface.hpp>

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
    boost::optional<unsigned> startFrame_;
    boost::optional<unsigned> endFrame_;
    boost::optional<unsigned> maxFrames_;
    boost::optional<double> startTime_;
    boost::optional<double> endTime_;
    boost::optional<double> maxTime_;
    std::string fontFile_;
    unsigned maxCols_;
    unsigned maxRows_;
    unsigned threads_;
    bool renderAll_;
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
        ("max-frames", value(&maxFrames_), "max number of video frames")
        ("start-time", value(&startTime_), "starting video position in seconds")
        ("end-time", value(&endTime_), "final video position in seconds")
        ("max-time", value(&maxTime_), "max video time")
        ("font-file,f", value(&fontFile_), "font file")
        ("cols", value(&maxCols_)->default_value(79), "suggested number of text columns")
        ("rows", value(&maxRows_)->default_value(49), "suggested number of text rows")
        ("threads", value(&threads_)->default_value(0), "number of worker threads (0 = auto)")
        ("render-all", bool_switch(&renderAll_), "render all frames")
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
    cv::VideoCapture capture(inputFile_);
    if (!capture.isOpened())
        return -1;

    unsigned frame_width = static_cast<unsigned>(capture.get(CV_CAP_PROP_FRAME_WIDTH));
    unsigned frame_height = static_cast<unsigned>(capture.get(CV_CAP_PROP_FRAME_HEIGHT));

    KG::Ascii::FontImage font;
    if (!font.load(fontFile_))
        return -1;
    unsigned char_width = font.glyphWidth();
    unsigned char_height = font.glyphHeight();

    unsigned hint_width = maxCols_ * char_width;
    unsigned hint_height = maxRows_ * char_height;
    unsigned out_width, out_height;
    if (hint_width * frame_height / frame_width < hint_height) {
        out_width = hint_width;
        out_height = out_width * frame_height / frame_width;
    } else {
        out_height = hint_height;
        out_width = out_height * frame_width / frame_height;
    }

    unsigned col_count = (out_width + char_width - 1) / char_width;
    unsigned row_count = (out_height + char_height - 1) / char_height;

    KG::Ascii::TextSurface text(row_count, col_count);
    KG::Ascii::FontPCAnalyzer pcanalyzer(font);
    KG::Ascii::FontPCA pca(pcanalyzer, 10);
    //KG::Ascii::PolicyBasedGlyphMatcher<KG::Ascii::SquaredEuclideanDistance> matcher(font);
    //KG::Ascii::PolicyBasedGlyphMatcherContext<KG::Ascii::MeansDistance> matcher_ctx(font);
    KG::Ascii::PcaGlyphMatcherContext matcher_ctx(pca);
    KG::Ascii::DynamicAsciifier asciifier(matcher_ctx);
    if (threads_ == 1) {
        asciifier.setSequential();
    } else {
        asciifier.setParallel(threads_);
    }

    cout << "video width " << frame_width << "\n";
    cout << "video height " << frame_height << "\n";
    cout << "video frame count " << capture.get(CV_CAP_PROP_FRAME_COUNT) << "\n";
    cout << "video fps " << capture.get(CV_CAP_PROP_FPS) << "\n";
    cout << "output width " << out_width << "\n";
    cout << "output height " << out_height << "\n";
    cout << "output columns " << col_count << "\n";
    cout << "output rows " << row_count << "\n";
    cout << "worker threads " << asciifier.threadCount() << "\n";

    if (startFrame_) {
        capture.set(CV_CAP_PROP_POS_FRAMES, *startFrame_);
    } else if (startTime_) {
        capture.set(CV_CAP_PROP_POS_MSEC, *startTime_ * 1000.0);
    }
    if (!capture.grab())
        return 0;

    Console con;
    con.setup(row_count, col_count);

    startFrame_ = static_cast<unsigned>(capture.get(CV_CAP_PROP_POS_FRAMES));
    startTime_ = capture.get(CV_CAP_PROP_POS_MSEC) / 1000.0;
    unsigned frame_count = 0;

    boost::timer timer;

    while (true) {
        unsigned current_frame = static_cast<unsigned>(capture.get(CV_CAP_PROP_POS_FRAMES));
        if (maxFrames_ && (current_frame - *startFrame_) >= *maxFrames_)
            break;
        if (endFrame_ && current_frame >= *endFrame_)
            break;

        double current_time = capture.get(CV_CAP_PROP_POS_MSEC) / 1000.0;
        if (endTime_ && current_time >= *endTime_)
            break;
        if (maxTime_ && (current_time - *startTime_) >= *maxTime_)
            break;

        double elapsed_time = timer.elapsed();

        if (!renderAll_ && current_time - *startTime_ < elapsed_time) {
            if (!capture.grab())
                break;
            continue;
        }

        cv::Mat capture_frame;
        if (!capture.retrieve(capture_frame))
            break;
        cv::Mat scaled_frame;
        if (frame_width == out_width && frame_height == out_height) {
            scaled_frame = capture_frame;
        } else {
            cv::resize(capture_frame, scaled_frame, cv::Size(out_width, out_height));
        }

        cv::Mat gray_frame;
        cv::cvtColor(scaled_frame, gray_frame, CV_BGR2GRAY);

        assert(gray_frame.dims == 2);
        assert(static_cast<unsigned>(gray_frame.cols) == out_width);
        assert(static_cast<unsigned>(gray_frame.rows) == out_height);
        assert(gray_frame.type() == CV_8UC1);

        KG::Ascii::Surface8c gray_surface(out_width, out_height, 
                gray_frame.data, gray_frame.step[0]);

        text.clear();
        asciifier.generate(gray_surface, text);

        con.display(text);
        frame_count++;

        elapsed_time = timer.elapsed();
        if (!renderAll_ && current_time - *startTime_ > elapsed_time) {
            double dtime = (current_time - *startTime_) - elapsed_time;
            dtime *= 0.8;
            double dtime_sec = 0.0;
            double dtime_frac = modf(dtime, &dtime_sec);
            boost::xtime xt;
            boost::xtime_get(&xt, boost::TIME_UTC);
            xt.sec += static_cast<int>(dtime_sec);
            xt.nsec += static_cast<int>(dtime_frac * 1000000000);
            boost::thread::sleep(xt);
        }
        if (!capture.grab())
            break;
    }

    double total_time = timer.elapsed();

    endFrame_ = static_cast<unsigned>(capture.get(CV_CAP_PROP_POS_FRAMES));
    endTime_ = capture.get(CV_CAP_PROP_POS_MSEC) / 1000.0;

    cout << "total frames " << (*endFrame_ - *startFrame_) << "\n";
    cout << "displayed frames " << frame_count << "\n";
    cout << "skipped frames " << (*endFrame_ - *startFrame_ - frame_count) << "\n";
    cout << "total video time " << (*endTime_ - *startTime_) << "\n";
    cout << "processing time " << total_time << "\n";
    cout << "processing time / frame " << total_time / frame_count << "\n";

    return 0;
}

