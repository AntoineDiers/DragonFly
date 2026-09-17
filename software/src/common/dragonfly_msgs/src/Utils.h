#pragma once

#include <string>
#include <vector>
#include <optional>
#include <filesystem>
#include <map>
#include <fstream>
#include <iostream>
#include <regex>

struct MsgFieldDesc
{
    std::string type;
    std::string name;
};

struct MsgDesc
{
    std::filesystem::path header_filepath;
    std::string class_name;
    std::vector<std::string> includes;
    std::vector<std::string> ns;
    std::vector<MsgFieldDesc> fields;
};

struct EnumDesc
{
    std::filesystem::path header_filepath;
    std::string enum_name;
    std::vector<std::string> ns;
    std::vector<std::string> values;
};