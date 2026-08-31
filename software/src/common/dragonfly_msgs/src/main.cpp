#include "Parsing.h"
#include "Generation.h"

#include <iostream>

int main(int argc, char* argv[])
{
    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " <dragonfly_msgs root path>\n";
        return 1;
    }

    std::filesystem::path root_path = argv[1];

    generateHeaderFiles(parseMsgFiles(root_path), root_path);

    return 0;
}