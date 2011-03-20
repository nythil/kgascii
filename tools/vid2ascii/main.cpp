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
#include <kgascii/dynamicasciifier.hpp>
#include <kgascii/policybasedglyphmatcher.hpp>
#include <kgascii/squaredeuclideandistance.hpp>
#include <kgascii/meansdistance.hpp>
#include <windows.h>

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
    int maxCols_;
    int maxRows_;
    int threads_;
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

class WindowsConsole: boost::noncopyable
{
public:
    WindowsConsole();

    ~WindowsConsole();

    void setup(int rows, int cols);

    void display(const KG::Ascii::TextSurface& text);

    void resize(HANDLE hnd, COORD buf, SMALL_RECT win);

private:
    HANDLE hndSavedOutput_;
    CONSOLE_SCREEN_BUFFER_INFO csbiSaved_;
    CONSOLE_CURSOR_INFO cciSaved_;
    HANDLE hndOutput_;
};

WindowsConsole::WindowsConsole()
{
    hndSavedOutput_ = GetStdHandle(STD_OUTPUT_HANDLE);
    GetConsoleScreenBufferInfo(hndSavedOutput_, &csbiSaved_);
    GetConsoleCursorInfo(hndSavedOutput_, &cciSaved_);
    hndOutput_ = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
}

WindowsConsole::~WindowsConsole()
{
    SetConsoleActiveScreenBuffer(hndSavedOutput_);
    SetConsoleTextAttribute(hndSavedOutput_, csbiSaved_.wAttributes);
    SetConsoleCursorInfo(hndSavedOutput_, &cciSaved_);
    resize(hndSavedOutput_, csbiSaved_.dwSize, csbiSaved_.srWindow);
    CloseHandle(hndOutput_);
}

void WindowsConsole::resize(HANDLE hnd, COORD buf, SMALL_RECT win)
{
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hnd, &csbi);
    if (csbi.dwSize.X < buf.X) {
        COORD xy = { buf.X, csbi.dwSize.Y };
        SetConsoleScreenBufferSize(hnd, xy);
        GetConsoleScreenBufferInfo(hnd, &csbi);
    }
    if (csbi.dwSize.Y < buf.Y) {
        COORD xy = { csbi.dwSize.X, buf.Y };
        SetConsoleScreenBufferSize(hnd, xy);
        GetConsoleScreenBufferInfo(hnd, &csbi);
    }
    if (csbi.srWindow.Left != win.Left || csbi.srWindow.Top != win.Top || csbi.srWindow.Right != win.Right || csbi.srWindow.Bottom != win.Bottom) {
        SetConsoleWindowInfo(hnd, TRUE, &win);
        GetConsoleScreenBufferInfo(hnd, &csbi);
    }
    if (csbi.dwSize.X != buf.X || csbi.dwSize.Y != buf.Y) {
        SetConsoleScreenBufferSize(hnd, buf);
        GetConsoleScreenBufferInfo(hnd, &csbi);
    }
}

void WindowsConsole::setup(int rows, int cols)
{
    SetConsoleActiveScreenBuffer(hndOutput_);
    COORD xy = { cols, rows };
    SMALL_RECT sr = { 0, 0, cols - 1, rows - 1 };
    resize(hndOutput_, xy, sr);
}

void WindowsConsole::display(const KG::Ascii::TextSurface& text)
{
    for (int r = 0; r < text.rows(); ++r) {
        for (int c = 0; c < text.cols(); ++c)
            assert(32 <= text(r, c) && text(r, c) <= 127);
        COORD xy = { 0, r };
        DWORD written;
        WriteConsoleOutputCharacterA(hndOutput_, text.row(r), text.cols(), xy, &written);
    }
}

class PcaGlyphMatcherContext;

class PcaGlyphMatcher: public KG::Ascii::GlyphMatcher
{
public:
    explicit PcaGlyphMatcher(const PcaGlyphMatcherContext& c);

public:
    const KG::Ascii::GlyphMatcherContext& context() const;

    char match(const boost::gil::gray8c_view_t& imgv);

private:
    const PcaGlyphMatcherContext& context_;
    Eigen::VectorXf imgvec_;
    Eigen::VectorXf components_;
};

class PcaGlyphMatcherContext: public KG::Ascii::GlyphMatcherContext
{
    friend class PcaGlyphMatcher;

public:
    explicit PcaGlyphMatcherContext(const KG::Ascii::FontImage& f)
        :KG::Ascii::GlyphMatcherContext(f)
        ,charcodes_(font().charcodes())
    {
        glyphSize_ = font().glyphWidth() * font().glyphHeight();
        Eigen::MatrixXd input_samples(glyphSize_, charcodes_.size());
        for (size_t ci = 0; ci < charcodes_.size(); ++ci) {
            boost::gil::gray8c_view_t glyph_view = font().getGlyph(charcodes_[ci]);
            for (int yy = 0; yy < font().glyphHeight(); ++yy) {
                for (int xx = 0; xx < font().glyphWidth(); ++xx) {
                    input_samples(yy * font().glyphWidth() + xx, ci) = glyph_view(xx, yy);
                }
            }
        }
        cout << "input_samples\n";
        Eigen::VectorXd mean_sample = input_samples.rowwise().sum() / charcodes_.size();
        Eigen::MatrixXd normalized_samples = input_samples.colwise() - mean_sample;
        cout << "normalized_samples\n";

        Eigen::MatrixXd covariance(glyphSize_, glyphSize_);
        covariance.setZero();
        covariance.selfadjointView<Eigen::Lower>().rankUpdate(normalized_samples, 1.0 / (normalized_samples.cols() - 1));
        cout << "covariance\n";

        Eigen::SelfAdjointEigenSolver<Eigen::MatrixXd> eigen_solver(covariance);
        Eigen::VectorXd eigvals_tmp = eigen_solver.eigenvalues();
        Eigen::VectorXd eigvals = eigvals_tmp.cwiseMax(Eigen::VectorXd::Zero(glyphSize_));
        Eigen::MatrixXd eigvecs = eigen_solver.eigenvectors();
        cout << "eigenvalues\n";

        Eigen::VectorXd featvals = eigvals.tail(10).reverse();
        Eigen::MatrixXd featvecs = eigvecs.rightCols(10).rowwise().reverse();
        for (int i = 0; i < 10; ++i) {
            //featvecs.col(i) *= featvals(i) / featvals.sum();
        }

        Eigen::MatrixXd glyphs = featvecs.transpose() * normalized_samples;
        cout << "glyphs\n";

        mean_ = mean_sample.cast<float>();
        energies_ = featvals.cast<float>();
        cout << "energies: " << energies_ << "\n";
        cout << "perc: " << featvals.sum() / eigvals.sum() << "\n";
        features_ = featvecs.cast<float>();
        cout << "features-min: " << features_.colwise().minCoeff() << "\n";
        cout << "features-max: " << features_.colwise().maxCoeff() << "\n";
        glyphs_ = glyphs.cast<float>();
        cout << "glyphs-min: " << glyphs_.rowwise().minCoeff() << "\n";
        cout << "glyphs-max: " << glyphs_.rowwise().maxCoeff() << "\n";
    }

public:
    PcaGlyphMatcher* createMatcher() const
    {
        return new PcaGlyphMatcher(*this);
    }

private:
    std::vector<int> charcodes_;
    int glyphSize_;
    Eigen::VectorXf mean_;
    Eigen::VectorXf energies_;
    Eigen::MatrixXf features_;
    Eigen::MatrixXf glyphs_;
};

PcaGlyphMatcher::PcaGlyphMatcher(const PcaGlyphMatcherContext& c)
    :KG::Ascii::GlyphMatcher()
    ,context_(c)
    ,imgvec_(context_.glyphSize_)
    ,components_(10)
{
}

const KG::Ascii::GlyphMatcherContext& PcaGlyphMatcher::context() const
{
    return context_;
}

char PcaGlyphMatcher::match(const boost::gil::gray8c_view_t& imgv)
{
    assert(imgv.width() <= context_.cellWidth());
    assert(imgv.height() <= context_.cellHeight());

    imgvec_.setZero();
    for (size_t y = 0; y < imgv.height(); ++y) {
        for (size_t x = 0; x < imgv.width(); ++x) {
            imgvec_(x + context_.cellWidth() * y) = imgv(x, y);
        }
    }

    components_ = (imgvec_ - context_.mean_).transpose() * context_.features_;
    int cc_min = ' ';
    (context_.glyphs_.colwise() - components_).colwise().squaredNorm().minCoeff(&cc_min);
    return (char)context_.charcodes_.at(cc_min);
}


int VideoToAscii::doExecute()
{
    using namespace boost::gil;

    cv::VideoCapture capture(inputFile_);
    if (!capture.isOpened())
        return -1;

    int frame_width = static_cast<int>(capture.get(CV_CAP_PROP_FRAME_WIDTH));
    int frame_height = static_cast<int>(capture.get(CV_CAP_PROP_FRAME_HEIGHT));

    KG::Ascii::FontImage font;
    if (!font.load(fontFile_))
        return -1;
    int char_width = font.glyphWidth();
    int char_height = font.glyphHeight();

    int hint_width = maxCols_ * char_width;
    int hint_height = maxRows_ * char_height;
    int out_width, out_height;
    if (hint_width * frame_height / frame_width < hint_height) {
        out_width = hint_width;
        out_height = out_width * frame_height / frame_width;
    } else {
        out_height = hint_height;
        out_width = out_height * frame_width / frame_height;
    }

    int col_count = (out_width + char_width - 1) / char_width;
    int row_count = (out_height + char_height - 1) / char_height;

    KG::Ascii::TextSurface text(row_count, col_count);
    //KG::Ascii::PolicyBasedGlyphMatcher<KG::Ascii::SquaredEuclideanDistance> matcher(font);
    //KG::Ascii::PolicyBasedGlyphMatcherContext<KG::Ascii::MeansDistance> matcher_ctx(font);
    PcaGlyphMatcherContext matcher_ctx(font);
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

    WindowsConsole con;
    con.setup(row_count, col_count);

    startFrame_ = static_cast<int>(capture.get(CV_CAP_PROP_POS_FRAMES));
    startTime_ = capture.get(CV_CAP_PROP_POS_MSEC) / 1000.0;
    int frame_count = 0;

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

        double elapsed_time = timer.elapsed();

        if (current_time - *startTime_ < elapsed_time) {
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
        assert(gray_frame.cols == out_width);
        assert(gray_frame.rows == out_height);
        assert(gray_frame.type() == CV_8UC1);

        gray8c_view_t gray_view = interleaved_view(out_width, out_height, 
                reinterpret_cast<gray8c_ptr_t>(gray_frame.data), 
                gray_frame.step[0]);

        text.clear();
        asciifier.generate(gray_view, text);

        con.display(text);
        frame_count++;

        elapsed_time = timer.elapsed();
        if (current_time - *startTime_ > elapsed_time) {
            double dtime = (current_time - *startTime_) - elapsed_time;
            Sleep(static_cast<int>(dtime * 800));
        }
        if (!capture.grab())
            break;
    }

    double total_time = timer.elapsed();

    endFrame_ = static_cast<int>(capture.get(CV_CAP_PROP_POS_FRAMES));
    endTime_ = capture.get(CV_CAP_PROP_POS_MSEC) / 1000.0;

    cout << "total frames " << (*endFrame_ - *startFrame_) << "\n";
    cout << "displayed frames " << frame_count << "\n";
    cout << "skipped frames " << (*endFrame_ - *startFrame_ - frame_count) << "\n";
    cout << "total video time " << (*endTime_ - *startTime_) << "\n";
    cout << "processing time " << total_time << "\n";
    cout << "processing time / frame " << total_time / frame_count << "\n";

    return 0;
}

