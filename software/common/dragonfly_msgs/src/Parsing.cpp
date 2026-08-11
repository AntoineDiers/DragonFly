#include "Generation.h"

static std::map<std::string, std::string> BASE_TYPES = 
{
    {"bool", "bool"},
    {"f32", "float"},
    {"f64", "double"},
    {"u8",  "uint8_t"},
    {"u16", "uint16_t"},
    {"u32", "uint32_t"},
    {"u64", "uint64_t"}
};

std::vector<std::string> split(const std::string& str)
{
    std::vector<std::string> result;

    size_t i = 0;

    while (i < str.size())
    {
        // Ignore espaces et tabs
        while (i < str.size() && (str[i] == ' ' || str[i] == '\t'))
            i++;

        if (i >= str.size())
            break;

        size_t begin = i;

        // Cherche la fin du token
        while (i < str.size() && str[i] != ' ' && str[i] != '\t')
            i++;

        result.push_back(str.substr(begin, i - begin));
    }

    return result;
}


std::optional<MsgFieldDesc> parseMsgField(const std::string& line)
{
    if(line.empty() || line[0] == '#')
    {
        return std::nullopt;
    }

    MsgFieldDesc res;

    auto tokens = split(line);
    res.name = tokens[1];

    if (tokens.size() != 2)
        throw std::runtime_error("Invalid field declaration: " + line);

    std::string type = tokens[0];

    // Gestion d'un tableau : u8[10]
    const size_t bracket = type.find('[');

    if (bracket != std::string::npos)
    {
        const size_t closing = type.find(']', bracket);

        if (closing == std::string::npos)
            throw std::runtime_error("Missing closing bracket: " + type);

        res.array_size = static_cast<uint32_t>(
            std::stoul(type.substr(
                bracket + 1,
                closing - bracket - 1)));

        type = type.substr(0, bracket);
    }

    auto it = BASE_TYPES.find(type);
    res.is_base_type = it != BASE_TYPES.end();
    res.type = res.is_base_type ? it->second : std::regex_replace(type, std::regex("/"), "::");
    
    return res;
}

MsgDesc parseMsgFile(const std::filesystem::path &filepath, const std::filesystem::path &root_path)
{
    std::cout << "Parsing msg file : " << filepath.string() << " ...\n";

    MsgDesc res;
    
    // Find the header file path 
    std::filesystem::path msg_files_root_path = root_path / "msg";
    std::filesystem::path hdr_files_root_path = root_path / "include" / "msg";

    std::filesystem::path relative_path = std::filesystem::relative(filepath, root_path / "msg");
    res.header_filepath = root_path / "include" / "dragonfly_msgs" / "msg" / relative_path;
    res.header_filepath.replace_extension(".h");

    // Find the class name
    res.class_name = filepath.stem().string();
    res.ns = {"dragonfly_msgs", "msg"};
    for(auto& folder : relative_path.parent_path())
    {
        res.ns.push_back(folder.filename().string());
    }

    // Parse fields
    std::ifstream file(filepath);
    std::string line;
    while (std::getline(file, line)) 
    {
        std::optional<MsgFieldDesc> field = parseMsgField(line);
        if(field.has_value()) 
        {
            res.fields.push_back(field.value());
        }
    }

    return res;
}

std::vector<MsgDesc> parseMsgFiles(const std::filesystem::path &root_path)
{
    std::vector<MsgDesc> res;

    for (const auto& entry : std::filesystem::recursive_directory_iterator(root_path / "msg")) 
    {
        if (entry.is_regular_file() && entry.path().extension() == ".msg")
        {
            res.push_back(parseMsgFile(entry.path(), root_path));
        }
    }

    return res;
}