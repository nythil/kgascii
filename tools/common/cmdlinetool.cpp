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

#include "cmdlinetool.hpp"
#include <iostream>
#include <stdexcept>
#include <boost/format.hpp>

int CmdlineTool::execute(int argc, const char* const argv[])
{
    try {
        parseArgs(argc, argv);

        if (vm_.count("help") || !processArgs()) {
            printUsage(argv[0]);
            return 0;
        }
    } catch (const std::logic_error& e) {
        std::cerr << "syntax error: " << e.what() << "\n";
        return -1;
    } catch (const std::exception& e) {
        std::cerr << e.what() << "\n";
        return -1;
    }

    return doExecute();
}

CmdlineTool::CmdlineTool(const std::string& caption)
    :desc_(caption)
{
    desc_.add_options()
        ("help", "help message")
    ;
}

CmdlineTool::~CmdlineTool()
{
}

bool CmdlineTool::processArgs()
{
    return true;
}

void CmdlineTool::printUsage(const char* prog_name)
{
    std::cout << "Usage: " << prog_name << "\n";
    std::cout << desc_;
}

void CmdlineTool::requireOption(const char* name)
{
    using boost::format;
    if (!vm_.count(name) || vm_[name].defaulted())
        throw std::logic_error(str(format("missing option '%1%'") % name));
}

void CmdlineTool::parseArgs(int argc, const char* const argv[])
{
    using namespace boost::program_options;
    command_line_parser parser(argc, argv);
    parser.options(desc_);
    parser.positional(posDesc_);
    store(parser.run(), vm_);
    notify(vm_);
}

