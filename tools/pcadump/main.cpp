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
#include <common/cmdline_tool.hpp>
#include "render_pca_command.hpp"


class PcaDump: public CmdlineTool
{
public:
    PcaDump();

protected:
    int doExecute();
};


int main(int argc, char* argv[])
{
    return PcaDump().execute(argc, argv);
}


PcaDump::PcaDump()
    :CmdlineTool("Options")
{
    using namespace boost::program_options;
    desc_.add_options()
        ("font-file,f", value<std::string>()->required(), "input dsc file")
        ("nfeatures,n", value<unsigned>()->required(), "number of features to extract")
        ("output-dsc", value<std::string>(), "output reconstructed dsc file")
        ("output-features", value<std::string>(), "output extracted feature masks")
    ;
    posDesc_.add("font-file", 1);
    posDesc_.add("nfeatures", 1);
}

int PcaDump::doExecute()
{
    RenderPcaCommand::Parameters params;
    params.font_file = vm_["font-file"].as<std::string>();
    params.feature_cnt = vm_["nfeatures"].as<unsigned>();
    if (!vm_["output-dsc"].empty()) {
        params.reconstructed_font_file = vm_["output-dsc"].as<std::string>();
    }
    if (!vm_["output-features"].empty()) {
        params.features_file = vm_["output-features"].as<std::string>();
    }

    RenderPcaCommand cmd(std::clog);
    cmd.execute(params);

    return 0;
}
