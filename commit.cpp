#include <iostream>
#include <string>
#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;

std::string get_commit_id() {
    fs::path counter_file = ".gitpta/counter";
    int curr_id = 1;
    if (fs::exists(counter_file)) {
        std::ifstream in(counter_file);
        in >> curr_id;
        in.close();
    }
    std::ofstream out(counter_file, std::ios::trunc);
    out << curr_id + 1 << "\n";
    out.close();
    return std::to_string(curr_id);
}

int command_commit(int argc, char** argv) {
    std::string message = "no message";
    if (argc >= 4 && std::string(argv[2]) == "-m") {
        message = argv[3];
    } else {
        std::cerr << "usage: gitpta commit -m \"messasge\"\n";
        return 6;
    }

    fs::path gitpta_dir = ".gitpta";
    fs::path stage_dir = gitpta_dir / "staging";
    fs::path commits_dir = gitpta_dir / "commits";
    fs::path branches_dir = gitpta_dir / "branches";
    fs::path head_file = gitpta_dir / "head";

    if (!fs::exists(stage_dir) || fs::is_empty(stage_dir)) {
        std::cout << "nothing to commit\n";
        return 0;
    }

    std::string commit_id = get_commit_id();
    fs::path new_commit_dir = commits_dir / commit_id;
    try {
        fs::create_directories(new_commit_dir);
        for (const auto& entry : fs::directory_iterator(stage_dir)) {
            fs::path dest = new_commit_dir / entry.path().filename();
            fs::copy(entry.path(), dest, fs::copy_options::recursive | fs::copy_options::overwrite_existing);
        }
        fs::remove_all(stage_dir);
        fs::create_directories(stage_dir);

        std::ofstream meta_out(new_commit_dir / ".meta");
        meta_out << "message: " << message << "\n";
        std::string parent_id = "root";
        if (fs::exists(head_file)) {
            std::ifstream head_in(head_file);
            std::getline(head_in, parent_id);
        }
        meta_out << "parent: " << parent_id << "\n";
        meta_out.close();
        std::ofstream head_out(head_file, std::ios::trunc);
        head_out << commit_id;
        head_out.close();
        std::cout << commit_id << ": " << message << "\n";
    } catch (...) {
        std::cerr << "error during create commit\n";
        return 7;
    }
    return 0;
}

