#include <iostream>
#include <filesystem>

namespace fs = std::filesystem;

int command_add(int argc, char** argv) {
    if (argc < 3) {
        std::cerr << "usage: gitpta add <file>\n";
        return 1;
    }

    fs::path gitpta_dir = ".gitpta";
    fs::path stage_dir = gitpta_dir / "staging";
    if (!fs::exists(".gitpta")) {
        std::cerr << "not gitpta repository\n";
        return 2;
    }

    fs::create_directories(stage_dir);
    fs::path source(argv[2]);
    if (!fs::exists(source)) {
        std::cerr << "error: " << argv[2] << " did not match any files\n";
        return 3;
    }
    try {
        fs::path destination = stage_dir / source.filename();
        fs::copy(source, destination, fs::copy_options::overwrite_existing);
        std::cout << "added " << argv[2] << " to staging area\n";
    } catch (...) {
        std::cerr << "error during copying file " << argv[2];
        return 4;
    }
    return 0;
}

