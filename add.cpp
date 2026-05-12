#include "add.hh"
#include "ignore.hh"
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
        if (fs::is_directory(source)) {
            for (auto& entry : fs::recursive_directory_iterator(source)) {
                if (entry.is_regular_file() && !is_ignored(entry.path())) {
                    if (entry.path().string().find(".gitpta") != std::string::npos) {
                        continue;
                    }

                    fs::path dest_dir = stage_dir / fs::relative(entry.path(), fs::current_path());
                    fs::create_directories(dest_dir.parent_path());
                    fs::copy(entry.path(), dest_dir, fs::copy_options::overwrite_existing);
                    std::cout << "add " << fs::relative(entry.path(), fs::current_path()).string() << " to staging area\n";
                }
            }
        } else {
            if (!is_ignored(source)) {
                fs::path dest_dir = stage_dir / source;
                fs::create_directories(dest_dir.parent_path());
                fs::copy(source, dest_dir, fs::copy_options::overwrite_existing);
                std::cout << "add " << argv[2] << " to staging area\n";
            }
        }
    } catch (...) {
        std::cerr << "error during copying file " << argv[2] << "\n";
        return 4;
    }
    return 0;
}


