#include "switch.hh"
#include <iostream>
#include <filesystem>
#include <fstream>
#include <string>

namespace fs = std::filesystem;

std::string get_commit_id(const std::string& target) {
    fs::path branch_file = fs::path(".gitpta/branches") / target;
    std::string target_commit_id;
    if (fs::exists(branch_file)) {
        std::ifstream in(branch_file);
        in >> target_commit_id;
        return target_commit_id;
    }
    return target;
}

int command_switch(int argc, char** argv) {
    if (argc < 3) {
        std::cerr << "usage: gitpta switch [-c] branch/commit_id\n";
        return 8;
    }
    std::string target = argv[2];
    bool create_branch = false;
    if (std::string(argv[2]) == "-c") {
        if (argc < 4) {
            std::cerr << "usage: gitpta switch -c branch_name\n";
            return 8;
        }
        create_branch = true;
        target = argv[3];
    }
    fs::path gitpta_dir = ".gitpta";
    if (!fs::exists(gitpta_dir)) {
        std::cerr << "error: not a gitpta repository\n";
        return 9;
    }
    std::string curr_head = "root";
    fs::path head_file = gitpta_dir / "head";
    if (fs::exists(head_file)) {
        std::ifstream in(head_file);
        in >> curr_head;
    }

    if (create_branch) {
        fs::path branch_file = gitpta_dir / "branches" / target;
        std::string current_commit_id = get_commit_id(curr_head);
        std::ofstream out(branch_file);
        out << current_commit_id << "\n";
        std::ofstream head_out(head_file);
        head_out << target << "\n";
        std::cout << "switch to a new branch " << target << "\n";
        return 0;
    }

    std::string target_commit_id = get_commit_id(target);
    fs::path target_commit_dir = gitpta_dir / "commits" / target_commit_id;
    if (target_commit_id != "root" && !fs::exists(target_commit_dir)) {
        std::cerr << "invalid id" << target << "\n";
        return 10;
    }

    std::string curr_commit_id = get_commit_id(curr_head);
    if (curr_commit_id != "root") {
        fs::path curr_commit_dir = gitpta_dir / "commits" / curr_commit_id;
        if (fs::exists(curr_commit_dir)) {
            for (const auto& entry : fs::directory_iterator(curr_commit_dir)) {
                if (entry.path().filename() != ".meta") {
                    fs::remove(entry.path().filename());
                }
            }
        }
    }

    if (target_commit_id != "root") {
        for (const auto& entry : fs::directory_iterator(target_commit_dir)) {
            if (entry.path().filename() != ".meta") {
                fs::copy(entry.path(), entry.path().filename(), fs::copy_options::overwrite_existing);
            }
        }
    }

    std::ofstream out(head_file, std::ios::trunc);
    out << target << "\n";
    std::cout << "switched to " << target << "\n";
    return 0;
}

