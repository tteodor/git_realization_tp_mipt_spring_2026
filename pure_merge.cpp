#include "pure_merge.hh"
#include "commit.hh"
#include <iostream>
#include <filesystem>
#include <fstream>
#include <set>
#include <vector>
#include <algorithm>

namespace fs = std::filesystem;



bool did_file_equal(const fs::path& p1, const fs::path& p2) {
    if (fs::file_size(p1) != fs::file_size(p2)) {
        return false;
    }
    std::ifstream f1(p1, std::ios::binary);
    std::ifstream f2(p2, std::ios::binary);
    return std::equal(std::istreambuf_iterator<char>(f1),std::istreambuf_iterator<char>(),std::istreambuf_iterator<char>(f2));
}


int command_pure_merge(int argc, char** argv) {
    try {
        if (argc < 3) {
            std::cerr << "usage: gitpta pure_merge <commit_id_to_merge_in>\n";
            return 6;
        }
        fs::path gitpta_dir = ".gitpta";
        if (!fs::exists(gitpta_dir)) {
            std::cerr << "error: not gitpta repository\n";
            return 2;
        }
        fs::path stage_dir = gitpta_dir / "staging";
        if (!fs::is_empty(stage_dir)) {
            std::cerr << "Merge abort! You have uncommited changes\n";
            return 4;
        }
        std::string target = argv[2];
        std::string head_id = "root";
        fs::path head_file = gitpta_dir / "head";
        std::ifstream head_in(gitpta_dir / "head");
        head_in >> head_id;
        if (head_id == "root") {
            std::cerr << "error: nothing to commit, because HEAD is root\n";
            return 2;
        }
        fs::path head_dir = gitpta_dir / "commits" / head_id;
        fs::path target_dir = gitpta_dir / "commits" / target;
        if (!fs::exists(target_dir)) {
            std::cerr << "error: commit " << target << "not exist\n";
            return 3;
        }
        for (const auto& entry : fs::directory_iterator(target_dir)) {
            std::string filename = entry.path().filename().string();
            if (filename != ".meta" && fs::exists(head_dir / filename) && !did_file_equal(head_dir / filename, entry.path())) {
                std::cerr << "Merge abort! Conflict in file: " << filename << "\n";
                return 4;
            }
        }
        fs::remove_all(stage_dir);
        fs::create_directories(stage_dir);
        for (const auto& entry : fs::directory_iterator(head_dir)) {
            if (entry.path().filename() != ".meta") {
                fs::copy(entry.path(), stage_dir / entry.path().filename());
            }
        }
        for (const auto& entry : fs::directory_iterator(target_dir)) {
            if (entry.path().filename() != ".meta" && !fs::exists(head_dir / entry.path().filename().string())) {
                fs::copy(entry.path(), stage_dir / entry.path().filename());
            }
        }
        std::cout << "No conflicts. Make commit\n";
        std::vector<char*> c_argv;
        c_argv.push_back(const_cast<char*>("gitpta"));
        c_argv.push_back(const_cast<char*>("commit"));
        c_argv.push_back(const_cast<char*>("-m"));
        std::string message = "Merge " + target + " into " + head_id;
        c_argv.push_back(const_cast<char*>(message.c_str()));
        return command_commit(4, c_argv.data());
    } catch (const std::exception& e) {
        std::cerr << "System error during merge: " << e.what() << "\n";
        return 8;
    }
}

