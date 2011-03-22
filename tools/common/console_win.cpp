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

#include "console.hpp"
#include <kgascii/text_surface.hpp>
#include <windows.h>


class ConsoleImpl: boost::noncopyable
{
public:
    ConsoleImpl();

    ~ConsoleImpl();

    void setup(int rows, int cols);

    void display(const KG::Ascii::TextSurface& text);

    void resize(HANDLE hnd, COORD buf, SMALL_RECT win);

    HANDLE hndSavedOutput_;
    CONSOLE_SCREEN_BUFFER_INFO csbiSaved_;
    CONSOLE_CURSOR_INFO cciSaved_;
    HANDLE hndOutput_;
};

ConsoleImpl::ConsoleImpl()
{
    hndSavedOutput_ = GetStdHandle(STD_OUTPUT_HANDLE);
    GetConsoleScreenBufferInfo(hndSavedOutput_, &csbiSaved_);
    GetConsoleCursorInfo(hndSavedOutput_, &cciSaved_);
    hndOutput_ = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
}

ConsoleImpl::~ConsoleImpl()
{
    SetConsoleActiveScreenBuffer(hndSavedOutput_);
    SetConsoleTextAttribute(hndSavedOutput_, csbiSaved_.wAttributes);
    SetConsoleCursorInfo(hndSavedOutput_, &cciSaved_);
    resize(hndSavedOutput_, csbiSaved_.dwSize, csbiSaved_.srWindow);
    CloseHandle(hndOutput_);
}

void ConsoleImpl::resize(HANDLE hnd, COORD buf, SMALL_RECT win)
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

void ConsoleImpl::setup(int rows, int cols)
{
    SetConsoleActiveScreenBuffer(hndOutput_);
    COORD xy = { cols, rows };
    SMALL_RECT sr = { 0, 0, cols - 1, rows - 1 };
    resize(hndOutput_, xy, sr);
}

void ConsoleImpl::display(const KG::Ascii::TextSurface& text)
{
    for (unsigned r = 0; r < text.rows(); ++r) {
        for (unsigned c = 0; c < text.cols(); ++c)
            assert(32 <= text(r, c) && text(r, c) <= 127);
        COORD xy = { 0, r };
        DWORD written;
        WriteConsoleOutputCharacterA(hndOutput_, text.row(r), text.cols(), xy, &written);
    }
}


Console::Console()
    :impl_(new ConsoleImpl)
{
}

Console::~Console()
{
}

void Console::setup(int rows, int cols)
{
    impl_->setup(rows, cols);
}

void Console::display(const KG::Ascii::TextSurface& text)
{
    impl_->display(text);
}

