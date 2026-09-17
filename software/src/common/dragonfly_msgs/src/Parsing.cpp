#include "Generation.h"
#include "Parsing.h"

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

    auto tokens = split(line);
    

    if (tokens.size() < 2)
        throw std::runtime_error("Invalid field declaration: " + line);

    return MsgFieldDesc
    {
        .type = tokens[0],
        .name = tokens[1]
    };
}

MsgDesc parseMsgFile(const std::filesystem::path &filepath, const std::filesystem::path &root_path)
{
    std::cout << "Parsing msg file : " << filepath.string() << " ...\n";

    MsgDesc res;
    
    // Find the header file path 
    std::filesystem::path msg_files_root_path = root_path / "msgs";
    std::filesystem::path hdr_files_root_path = root_path / "include" / "msgs";

    std::filesystem::path relative_path = std::filesystem::relative(filepath, root_path / "msgs");
    res.header_filepath = root_path / "include" / "dragonfly_msgs" / "msgs" / relative_path;
    res.header_filepath.replace_extension(".h");

    // Find the class name
    res.class_name = filepath.stem().string();
    res.ns = {"dragonfly_msgs", "msgs"};
    for(auto& folder : relative_path.parent_path())
    {
        res.ns.push_back(folder.filename().string());
    }

    // Parse fields
    std::ifstream file(filepath);
    std::string line;
    while (std::getline(file, line)) 
    {
        std::string include_str = "#include";
        if(line.size() >= include_str.size() && line.substr(0, include_str.size()) == include_str)
        {
            res.includes.push_back(line);
            continue;
        }
        
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

    for (const auto& entry : std::filesystem::recursive_directory_iterator(root_path / "msgs")) 
    {
        if (entry.is_regular_file() && entry.path().extension() == ".msg")
        {
            res.push_back(parseMsgFile(entry.path(), root_path));
        }
    }

    return res;

}

EnumDesc parseEnumFile(const std::filesystem::path &filepath, const std::filesystem::path &root_path)
{
    std::cout << "Parsing enum file : " << filepath.string() << " ...\n";

    EnumDesc res;
    
    // Find the header file path 
    std::filesystem::path msg_files_root_path = root_path / "enums";
    std::filesystem::path hdr_files_root_path = root_path / "include" / "enums";

    std::filesystem::path relative_path = std::filesystem::relative(filepath, root_path / "enums");
    res.header_filepath = root_path / "include" / "dragonfly_msgs" / "enums" / relative_path;
    res.header_filepath.replace_extension(".h");

    // Find the enum name
    res.enum_name = filepath.stem().string();
    res.ns = {"dragonfly_msgs", "enums"};
    for(auto& folder : relative_path.parent_path())
    {
        res.ns.push_back(folder.filename().string());
    }

    // Parse values
    std::ifstream file(filepath);
    std::string line;
    while (std::getline(file, line)) 
    {
        std::vector<std::string> split_line = split(line);
        if(split_line.size() > 0)
        {
            std::string value = split_line[0];
            if(value[0] != '#')
            {
                res.values.push_back(value);
            }
        }
    }

    return res;
}

std::vector<EnumDesc> parseEnumFiles(const std::filesystem::path &root_path)
{
    std::vector<EnumDesc> res;

    for (const auto& entry : std::filesystem::recursive_directory_iterator(root_path / "enums")) 
    {
        if (entry.is_regular_file() && entry.path().extension() == ".enum")
        {
            res.push_back(parseEnumFile(entry.path(), root_path));
        }
    }

    return res;
}