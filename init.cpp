#include <iostream>
#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;

int command_init(int argc, char** argv) {
    fs::path gitpta_dir = ".gitpta";
    try {
        fs::create_directories(gitpta_dir / "staging");
        fs::create_directories(gitpta_dir / "commits");
        fs::create_directories(gitpta_dir / "branches");

        std::ofstream head_out(gitpta_dir / "head");
        head_out << "root\n";
        head_out.close();

        std::ofstream counter_out(gitpta_dir / "counter");
        counter_out << 1 << "\n";
        counter_out.close();

        std::cout << "Initialized gitpta repository in " << fs::absolute(gitpta_dir) << "\n";
    } catch (...) {
        std::cerr << "error initializing repository\n";
        return 5;
    }
    return 0;
}

