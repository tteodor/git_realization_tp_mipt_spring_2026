#include "status.hh"
#include <iostream>
#include <filesystem>
#include <fstream>
#include <set>
#include <string>

namespace fs = std::filesystem;

int command_status(int argc, char** argv) {
    fs::path gitpta_dir = ".gitpta";
    if (!fs::exists(".gitpta")) {
        std::cerr << "not gitpta repository\n";
        return 2;
    }

    std::cout << "Current branch: ";
    std::cout << "main" << '\n' << '\n';

    std::string head_commit = "root";
    fs::path head_file = gitpta_dir / "head";
    if (fs::exists(head_file)) {
        std::ifstream in(head_file);
        in >> head_commit;
    }
    std::cout << "Current commit: ";
    std::cout << head_commit << '\n' << '\n';

    fs::path commit_dir = gitpta_dir / "commits" / head_commit;
    std::set<std::string> commit_files;

    if (head_commit != "root" && fs::exists(commit_dir)) {
        for (auto entry : fs::directory_iterator(commit_dir)) {
            if (entry.is_regular_file()) {
                commit_files.insert(entry.path().filename().string());
            }
        }
    }
    std::set<std::string> work_files;
    std::set<std::string> untracked;
    std::set<std::string> modified;
    std::set<std::string> deleted;
    for (auto entry : fs::directory_iterator(".")) {
        if (entry.is_regular_file()) {
            std::string filename = entry.path().filename().string();
            if (filename != ".gitpta" && filename != ".gitPTA" && filename != "gitpta" && filename != "gitPTA") {
                work_files.insert(filename);
                if (commit_files.find(filename) == commit_files.end()) {
                    untracked.insert(filename);
                } else {
                    fs::path commit_file_path = commit_dir / filename;
                    if (fs::file_size(entry.path()) != fs::file_size(commit_file_path) || fs::last_write_time(entry.path()) > fs::last_write_time(commit_file_path)) {
                        modified.insert(filename);
                    }
                }
            }
        }
    }
    for (auto file : commit_files) {
        if (file != ".meta" && work_files.find(file) == work_files.end()) {
            deleted.insert(file);
        }
    }
    if (!untracked.empty()) {
        std::cout << "Untracked files:" << '\n';
        for (auto file : untracked) {
            std::cout << file << '\n';
        }
        std::cout << '\n';
    }
    if (!modified.empty()) {
        std::cout << "Modified files:" << '\n';
        for (auto file : modified) {
            std::cout << file << '\n';
        }
        std::cout << '\n';
    }
    if (!deleted.empty()) {
        std::cout << "Deleted files:" << '\n';
        for (auto file : deleted) {
            std::cout << file << '\n';
        }
    }
    return 0;
}
