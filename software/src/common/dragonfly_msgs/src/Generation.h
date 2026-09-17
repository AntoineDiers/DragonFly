#pragma once

#include "Utils.h"

void generateHeaderFiles(
    const std::vector<MsgDesc>& msg_files, 
    const std::vector<EnumDesc>& enum_files, 
    const std::filesystem::path& root_path);