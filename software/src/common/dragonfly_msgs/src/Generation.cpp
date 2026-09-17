#include "Generation.h"

#include "../include/dragonfly_msgs/internal/base_types/BaseType.h"

static const std::string MSG_TEMPLATE = R"(
#pragma once

#include <dragonfly_msgs/internal/Common.h>

<INCLUDES>

<NAMESPACE_START>

class <NAME>
{
public:

<FIELDS>

    static constexpr uint32_t _SerialisationSizeBits = 
<FIELDS_SIZES>        0;
    static constexpr uint32_t _Id = <ID>;
    static constexpr uint32_t _IdSizeBits = <ID_SIZE_BITS>;
    static constexpr uint32_t _SizeBytes = serialisation::sizeBitsToBytes(_SerialisationSizeBits + _IdSizeBits);

    template<uint32_t BufferSize>
    void _serialise(Buffer<BufferSize>& buffer) const
    {
        static_assert(BufferSize >= _SizeBytes);

        serialisation::WBitBuffer bit_buffer(buffer.data.data());
        bit_buffer.write(_Id, _IdSizeBits);
        _serialise(bit_buffer);

        buffer.size = _SizeBytes;

        return;
    }

    template<uint32_t BufferSize>
    bool _deserialise(const Buffer<BufferSize>& buffer)
    {
        static_assert(BufferSize >= _SizeBytes);

        if(buffer.size != _SizeBytes) { return false; }
        
        serialisation::RBitBuffer bit_buffer(buffer.data.data());
        if(bit_buffer.read(_IdSizeBits) != _Id) { return false; }

        _deserialise(bit_buffer);
        return true;
    }
    
    void _serialise(serialisation::WBitBuffer& buffer) const
    {
<SERIALISE>    }

    void _deserialise(serialisation::RBitBuffer& buffer)
    {
<DESERIALISE>    }

    nlohmann::json _serialiseJson() const
    {
        nlohmann::json res;
<SERIALISE_JSON>        return res;
    }

};

<NAMESPACE_END>

)";

static const std::string ENUM_TEMPLATE = R"(
#pragma once

<NAMESPACE_START>

enum class <ENUM_NAME>
{
<ENUM_VALUES>    _COUNT
};

<NAMESPACE_END>

)";

static const std::string ROOT_HEADER_TEMPLATE = R"(
#pragma once

<INCLUDES>

namespace dragonfly_msgs
{
    namespace internal
    {
        constexpr uint32_t max(uint32_t a, uint32_t b)
        {
            return a > b ? a : b;
        }

        template<typename... Ts>
        constexpr uint32_t max(uint32_t a, uint32_t b, Ts... rest)
        {
            return max(max(a, b), rest...);
        }
    }
    
    static constexpr uint32_t MAX_MSG_SIZE = internal::max(
<SIZES>        0);
}
)";

void generateHeaderFiles(
    const std::vector<MsgDesc> &msg_files, 
    const std::vector<EnumDesc>& enum_files, 
    const std::filesystem::path &root_path)
{
    uint8_t id_size_bits = dragonfly_msgs::base_types::neededBits(msg_files.size());

    std::string root_header_content = ROOT_HEADER_TEMPLATE;
    std::string root_header_includes;
    std::string root_header_sizes;

    uint8_t id = 0;
    for(auto& msg : msg_files)
    {
        std::cout << "Generating header file : " << msg.header_filepath.string() << " ...\n";

        std::string include_path = std::filesystem::relative(msg.header_filepath, root_path / "include").string();
        root_header_includes += "#include <" + include_path + ">\n";
        root_header_sizes += "        ";
        for(auto& ns : msg.ns)
        {
            root_header_sizes += ns + "::";
        }
        root_header_sizes += msg.class_name + "::_SizeBytes,\n";

        std::string includes;
        std::string namespace_start;
        std::string fields;
        std::string fields_size;
        std::string serialise;
        std::string deserialise;
        std::string serialise_json;
        std::string namespace_end;

        for(auto& ns : msg.ns)
        {
            namespace_start += "namespace " + ns + " { \n";
            namespace_end += "}"; 
        }

        for(auto& include : msg.includes)
        {
            includes += include + "\n";
        }

        for(auto& field : msg.fields)
        {
            fields +=           "        " + field.type + " " + field.name + ";\n";
            fields_size +=      "        " + field.type + "::_SerialisationSizeBits + \n";
            serialise +=        "        " + field.name + "._serialise(buffer);\n";
            deserialise +=      "        " + field.name + "._deserialise(buffer);\n";
            serialise_json +=   "        res[\"" + field.name + "\"] = " + field.name + "._serialiseJson();\n";
        }

        std::string content = MSG_TEMPLATE;
        content = std::regex_replace(content, std::regex("<INCLUDES>"),         includes);
        content = std::regex_replace(content, std::regex("<NAMESPACE_START>"),  namespace_start);
        content = std::regex_replace(content, std::regex("<NAME>"),             msg.class_name);
        content = std::regex_replace(content, std::regex("<ID>"),               std::to_string(id));
        content = std::regex_replace(content, std::regex("<ID_SIZE_BITS>"),     std::to_string(id_size_bits));
        content = std::regex_replace(content, std::regex("<FIELDS>"),           fields);
        content = std::regex_replace(content, std::regex("<FIELDS_SIZES>"),     fields_size);
        content = std::regex_replace(content, std::regex("<SERIALISE>"),        serialise);
        content = std::regex_replace(content, std::regex("<DESERIALISE>"),      deserialise);
        content = std::regex_replace(content, std::regex("<SERIALISE_JSON>"),   serialise_json);
        content = std::regex_replace(content, std::regex("<NAMESPACE_END>"),    namespace_end);

        std::filesystem::create_directories(msg.header_filepath.parent_path());
        std::ofstream file(msg.header_filepath);
        file << content;

        if(!file.good())
        {
            throw std::runtime_error("Failed to write to " + msg.header_filepath.string());
        }

        id++;
    }

    for(auto& en : enum_files)
    {
        std::cout << "Generating header file : " << en.header_filepath.string() << " ...\n";
        
        std::string include_path = std::filesystem::relative(en.header_filepath, root_path / "include").string();
        root_header_includes += "#include <" + include_path + ">\n";

        std::string namespace_start;
        std::string values;
        std::string namespace_end;

        for(auto& ns : en.ns)
        {
            namespace_start += "namespace " + ns + " { \n";
            namespace_end += "}"; 
        }

        for(auto& value : en.values)
        {
            values += "    " + value + ",\n";
        }

        std::string content = ENUM_TEMPLATE;
        content = std::regex_replace(content, std::regex("<NAMESPACE_START>"),  namespace_start);
        content = std::regex_replace(content, std::regex("<ENUM_NAME>"),        en.enum_name);
        content = std::regex_replace(content, std::regex("<ENUM_VALUES>"),      values);
        content = std::regex_replace(content, std::regex("<NAMESPACE_END>"),    namespace_end);

        std::filesystem::create_directories(en.header_filepath.parent_path());
        std::ofstream file(en.header_filepath);
        file << content;

        if(!file.good())
        {
            throw std::runtime_error("Failed to write to " + en.header_filepath.string());
        }
    }

    root_header_content = std::regex_replace(root_header_content, std::regex("<INCLUDES>"), root_header_includes);
    root_header_content = std::regex_replace(root_header_content, std::regex("<SIZES>"),    root_header_sizes);

    std::filesystem::path root_header_filepath = root_path / "include" / "dragonfly_msgs" / "dragonfly_msgs.h";
    std::ofstream file(root_header_filepath);
    file << root_header_content;

    if(!file.good())
    {
        throw std::runtime_error("Failed to write to " + root_header_filepath.string());
    }
}