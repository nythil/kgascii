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
#include <boost/lexical_cast.hpp>
#include <boost/optional.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <common/cmdline_tool.hpp>
#include <common/validate_optional.hpp>
#include <common/console.hpp>
#include <common/video_player.hpp>
#include <kgascii/font_image.hpp>
#include <kgascii/glyph_matcher.hpp>
#include <kgascii/glyph_matcher_context.hpp>
#include <kgascii/dynamic_asciifier.hpp>
#include <kgascii/text_surface.hpp>
#include <kgascii/glyph_matcher_context_factory.hpp>

using std::cout;
using std::cerr;

class VideoToAscii: public CmdlineTool
{
    friend class MyVideoPlayer;
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
    bool showVideo_;
    std::string algorithm_;
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
        ("show-video", bool_switch(&showVideo_), "show original video")
        ("algorithm,a", value(&algorithm_)->default_value("pca"), "glyph matching algorithm")
    ;
    posDesc_.add("input-file", 1);
}

bool VideoToAscii::processArgs()
{
    requireOption("input-file");
    requireOption("font-file");
    requireOption("algorithm");
    
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

class MyVideoPlayer: public VideoPlayer
{
public:
    explicit MyVideoPlayer(const VideoToAscii* ctx, KG::Ascii::Asciifier* asc, Console* con)
        :context_(ctx)
        ,asciifier_(asc)
        ,console_(con)
    {
    }

protected:
    virtual void onLoaded()
    {
        cout << "initializing...\n";

        unsigned char_width = asciifier_->context()->cellWidth();
        unsigned char_height = asciifier_->context()->cellHeight();

        unsigned hint_width = context_->maxCols_ * char_width;
        unsigned hint_height = context_->maxRows_ * char_height;
        if (hint_width * frameHeight() / frameWidth() < hint_height) {
            outWidth_ = hint_width;
            outHeight_ = outWidth_ * frameHeight() / frameWidth();
        } else {
            outHeight_ = hint_height;
            outWidth_ = outHeight_ * frameWidth() / frameHeight();
        }

        cols_ = (outWidth_ + char_width - 1) / char_width;
        rows_ = (outHeight_ + char_height - 1) / char_height;

        cout << "video width " << frameWidth() << "\n";
        cout << "video height " << frameHeight() << "\n";
        cout << "video frame count " << frameCount() << "\n";
        cout << "output width " << outWidth_ << "\n";
        cout << "output height " << outHeight_ << "\n";
        cout << "output columns " << cols_ << "\n";
        cout << "output rows " << rows_ << "\n";
        cout << "worker threads " << asciifier_->threadCount() << "\n";

        if (context_->startFrame_) {
            cout << "positioning...\n";
            seekToFrame(context_->startFrame_.get());
        } else if (context_->startTime_) {
            cout << "positioning...\n";
            seekToTime(context_->startTime_.get());
        }
    }

    virtual void onPlaybackStart()
    {
        cout << "playback start\n";
        if (context_->renderAll_) {
            setCanDropFrames(false);
            setCanWaitForFrame(false);
        }
        if (context_->showVideo_) {
            cv::namedWindow("test", 1);
        }
        text_.resize(rows_, cols_);
        console_->setup(rows_, cols_);
    }

    virtual void onPlaybackEnd()
    {
        cout << "playback end\n";
        if (context_->showVideo_) {
            cv::destroyWindow("test");
        }
    }

    virtual bool onBeforeReadFrame(double tm_left)
    {
        if (context_->maxFrames_) {
            unsigned frm_cnt = currentFrameNo() - startFrameNo();
            if (frm_cnt >= context_->maxFrames_.get())
                return false;
        }
        if (context_->endFrame_) {
            if (currentFrameNo() >= context_->endFrame_.get())
                return false;
        }
        if (context_->maxTime_) {
            double tm_span = currentFrameTime() - startFrameTime();
            if (tm_span >= context_->maxTime_.get())
                return false;
        }
        if (context_->endTime_) {
            if (currentFrameTime() >= context_->endTime_.get())
                return false;
        }
        if (tm_left > 0 && context_->showVideo_) {
            if (cv::waitKey(1) >= 0)
                return false;
        }
        return true;
    }

    virtual void onFrameRead(cv::Mat frm, double tm_left)
    {
        cv::Mat scaled_frame;
        if (frameWidth() == outWidth_ && frameHeight() == outHeight_) {
            scaled_frame = frm;
        } else {
            cv::resize(frm, scaled_frame, cv::Size(outWidth_, outHeight_));
        }

        //cv::GaussianBlur(scaled_frame, scaled_frame, cv::Size(7,7), 1.5, 1.5);

        cv::cvtColor(scaled_frame, grayFrame_, CV_BGR2GRAY);
        //cv::equalizeHist(gray_frame, gray_frame);

        assert(grayFrame_.dims == 2);
        assert(static_cast<unsigned>(grayFrame_.cols) == outWidth_);
        assert(static_cast<unsigned>(grayFrame_.rows) == outHeight_);
        assert(grayFrame_.type() == CV_8UC1);

        KG::Ascii::Surface8c gray_surface(outWidth_, outHeight_,
                grayFrame_.data, grayFrame_.step[0]);

        text_.clear();
        asciifier_->generate(gray_surface, text_);
    }

    virtual void onFrameDisplay(cv::Mat frm)
    {
        if (context_->showVideo_) {
            cv::imshow("test", grayFrame_);
        }
        console_->display(text_);
    }

private:
    const VideoToAscii* context_;
    KG::Ascii::Asciifier* asciifier_;
    Console* console_;
    KG::Ascii::TextSurface text_;
    unsigned outWidth_;
    unsigned outHeight_;
    unsigned cols_;
    unsigned rows_;
    cv::Mat grayFrame_;
};

int VideoToAscii::doExecute()
{
    try {
        cout << "loading font\n";
        KG::Ascii::FontImage font;
        if (!font.load(fontFile_)) {
            cerr << "problem loading font\n";
            return 1;
        }
        cout << "creating glyph matcher\n";
        KG::Ascii::GlyphMatcherContextFactory matcher_ctx_factory;
        KG::Ascii::GlyphMatcherContext* matcher_ctx = matcher_ctx_factory.create(&font, algorithm_);
        assert(matcher_ctx);
        KG::Ascii::DynamicAsciifier asciifier(matcher_ctx);
        assert(asciifier.context() == matcher_ctx);
        if (threads_ == 1) {
            asciifier.setSequential();
        } else {
            asciifier.setParallel(threads_);
        }

        Console con;

        cout << "loading video\n";

        MyVideoPlayer vplayer(this, &asciifier, &con);
        if (!vplayer.load(inputFile_))
            return -1;

        vplayer.play();

        double frm_tm_spn = vplayer.currentFrameTime() - vplayer.startFrameTime();
        double plr_tm_spn = vplayer.currentTime() - vplayer.startTime();

        cout << "total frames " << vplayer.allReadFrames() << "\n";
        cout << "displayed frames " << vplayer.readFrames() << "\n";
        cout << "skipped frames " << (vplayer.allReadFrames() - vplayer.readFrames()) << "\n";
        cout << "total video time " << frm_tm_spn << "\n";
        cout << "processing time " << plr_tm_spn << "\n";
        cout << "processing time / frame " << plr_tm_spn / vplayer.readFrames() << "\n";
    } catch (std::exception& e) {
        cerr << "error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}

