#include "Generation.h"

static const std::string TEMPLATE = R"(
#pragma once

#include <dragonfly_msgs/Utils.h>

<INCLUDES>

<NAMESPACE_START>

class <NAME>
{
public:

<FIELDS>

    static constexpr uint32_t _BodySize = <SIZE> 0;
    static constexpr uint32_t _SerialisationSize = sizeof(MsgId) + _BodySize;
    static constexpr uint8_t _Id = <ID>;

    template<uint32_t BufferSize>
    bool _deserialise(const Buffer<BufferSize>& buffer)
    {
        static_assert(BufferSize >= _SerialisationSize);

        const uint8_t* ptr = buffer.data.data();

        if(buffer.size != _SerialisationSize) { return false; }
        MsgId id;
        internal::deserialiseField(id, ptr);
        if(id != _Id) { return false; }

        _deserialiseFields(ptr);

        return true;
    }
    
    template<uint32_t BufferSize>
    void _serialise(Buffer<BufferSize>& buffer) const
    {
        static_assert(BufferSize >= _SerialisationSize);

        uint8_t* ptr = buffer.data.data();
        internal::serialiseField(_Id, ptr);
        _serialiseFields(ptr);
        buffer.size = _SerialisationSize;
    }

    void _deserialiseFields(const uint8_t* &buffer)
    {
<DESERIALISE>
    }

    void _serialiseFields(uint8_t* &buffer) const
    {
<SERIALISE>
    }

    void _print(uint32_t n_tabs = 0) const
    {
        char tabs[4 * 5 + 1] = {0};
        for(uint32_t i = 0; i < std::min(4 * n_tabs, (uint32_t)sizeof(tabs) - 1); i++)
        {
            tabs[i] = ' ';
        }

        std::cout << "{\n";
<PRINT>
        std::cout << tabs << "}";
    }
};

<NAMESPACE_END>

)";

static const std::string ROOT_HEADER_TEMPLATE = R"(
#pragma once

#include <dragonfly_msgs/Utils.h>
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
    
    static constexpr uint32_t MAX_MSG_SIZE = sizeof(MsgId) + internal::max(
<SIZES>        0);
}
)";

void generateHeaderFiles(const std::vector<MsgDesc> &msg_files, const std::filesystem::path &root_path)
{
    if(msg_files.size() > 255)
    {
        throw std::runtime_error("Too many message files to generate, max = 255");
    }

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
        root_header_sizes += msg.class_name + "::_BodySize,\n";

        std::string content = TEMPLATE;
        
        std::string includes;
        std::string namespace_start;
        std::string size;
        std::string fields;
        std::string serialise;
        std::string deserialise;
        std::string print;
        std::string namespace_end;

        for(auto& ns : msg.ns)
        {
            namespace_start += "namespace " + ns + " { \n";
            namespace_end += "}"; 
        }

        for(auto& field : msg.fields)
        {
            if(!field.is_base_type)
            {
                std::string include_path = std::regex_replace(field.type, std::regex("::"), "/");
                includes += "#include <dragonfly_msgs/msg/" + include_path + ".h>\n";
            }

            std::string size_str = field.array_size.has_value() ? (std::to_string(field.array_size.value()) + "*") : "";
            size +=  
            (field.array_size.has_value() ? (std::to_string(field.array_size.value()) + "*") : "") +
            (field.is_base_type ? ("sizeof(" + field.type + ")") : field.type + "::_BodySize") + 
            " + "; 

            fields += "    " + field.type + " " + field.name + (field.array_size.has_value() ? ("[" + std::to_string(field.array_size.value()) + "]") : "") + ";\n";

            serialise += "        internal::" + std::string(field.array_size.has_value() ? "serialiseArrayField" : "serialiseField") + "<" + field.type + ">(" + field.name + ", buffer);\n";

            deserialise += "        internal::" + std::string(field.array_size.has_value() ? "deserialiseArrayField" : "deserialiseField") + "<" + field.type + ">(" + field.name + ", buffer);\n";
        
            print += "        internal::" + std::string(field.array_size.has_value() ? "printArrayField" : "printField") + "<" + field.type + ">(\"" + field.name + "\", " + field.name + ", n_tabs + 1);\n";
        }

        content = std::regex_replace(content, std::regex("<INCLUDES>"),         includes);
        content = std::regex_replace(content, std::regex("<NAMESPACE_START>"),  namespace_start);
        content = std::regex_replace(content, std::regex("<NAME>"),             msg.class_name);
        content = std::regex_replace(content, std::regex("<SIZE>"),             size);
        content = std::regex_replace(content, std::regex("<ID>"),               std::to_string(id));
        content = std::regex_replace(content, std::regex("<FIELDS>"),           fields);
        content = std::regex_replace(content, std::regex("<SERIALISE>"),        serialise);
        content = std::regex_replace(content, std::regex("<DESERIALISE>"),      deserialise);
        content = std::regex_replace(content, std::regex("<PRINT>"),            print);
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

    root_header_content = std::regex_replace(root_header_content, std::regex("<INCLUDES>"), root_header_includes);
    root_header_content = std::regex_replace(root_header_content, std::regex("<SIZES>"),    root_header_sizes);

    std::filesystem::path root_header_filepath = root_path / "include" / "dragonfly_msgs" / "msgs.h";
    std::ofstream file(root_header_filepath);
    file << root_header_content;

    if(!file.good())
    {
        throw std::runtime_error("Failed to write to " + root_header_filepath.string());
    }
}