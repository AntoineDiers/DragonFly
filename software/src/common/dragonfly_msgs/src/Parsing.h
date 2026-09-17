#pragma once

#include "Utils.h"

std::vector<MsgDesc> parseMsgFiles(const std::filesystem::path& root_path);
std::vector<EnumDesc> parseEnumFiles(const std::filesystem::path& root_path);