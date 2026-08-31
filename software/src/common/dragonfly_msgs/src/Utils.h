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
    bool is_base_type;
    std::string type;
    std::string name;
    std::optional<uint32_t> array_size;
};

/**
 * Example fields : 
 * 
 *      f32 my_float32
 *      u8[10] my_byte_array_of_len_10
 */

struct MsgDesc
{
    std::filesystem::path header_filepath;
    std::string class_name;
    std::vector<std::string> ns;
    std::vector<MsgFieldDesc> fields;
};