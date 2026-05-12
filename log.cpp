#include "log.hh"
#include <iostream>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>
#include <set>
#include <algorithm>

namespace fs = std::filesystem;

int command_log(int argc, char** argv) {
    bool oneline = false;
    bool all_branches = false;
    for (int i = 2; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--oneline") {
            oneline = true;
        } else if (arg == "--all") {
            all_branches = true;
        }
    }
    fs::path gitpta_dir = ".gitpta";
    if (!fs::exists(gitpta_dir)) {
        std::cerr << "error: not gitpta repository\n";
        return 2;
    }
    std::vector<std::string> lst;
    if (!all_branches) {
        std::string head_content = "root";
        fs::path head_file = gitpta_dir / "head";
        if (fs::exists(head_file)) {
            std::ifstream in(head_file);
            in >> head_content;
        }
        std::string id;
        fs::path branch_file = fs::path(".gitpta/branches") / head_content;
        if (fs::exists(branch_file)) {
            std::ifstream in(branch_file);
            in >> id;
        } else {
            id = head_content;
        }
        if (id != "root") {
            lst.push_back(id);
        }
    } else {
        fs::path branches_dir = gitpta_dir / "branches";
        if (fs::exists(branches_dir)) {
            for (const auto& entry : fs::directory_iterator(branches_dir)) {
                if (entry.is_regular_file()) {
                    std::ifstream branch_stream(entry.path());
                    std::string branch_tip;
                    if (branch_stream >> branch_tip && branch_tip != "root") {
                        lst.push_back(branch_tip);
                    }
                }
            }
        }
        std::string fallback_head;
        std::ifstream head_stream(gitpta_dir / "head");
        if (head_stream >> fallback_head) {
            fs::path check_branch = branches_dir / fallback_head;
            std::string real_id = fallback_head;
            if (fs::exists(check_branch)) {
                std::ifstream bs(check_branch);
                bs >> real_id;
            }
            if (real_id != "root" && std::find(lst.begin(), lst.end(), real_id) == lst.end()) {
                lst.push_back(real_id);
            }
        }
    }
    if (lst.empty()) {
        std::cout << "No commits made\n";
        return 0;
    }
    std::set<std::string> processed_nodes;
    std::vector<std::string> traversal_stack = lst;
    while (!traversal_stack.empty()) {
        std::string current_node = traversal_stack.back();
        traversal_stack.pop_back();
        if (current_node == "root" || processed_nodes.find(current_node) != processed_nodes.end()) {
            continue;
        }
        processed_nodes.insert(current_node);
        fs::path meta_path = gitpta_dir / "commits" / current_node / ".meta";
        std::string commit_msg = "<empty>";
        std::string parent_hash = "root";
        std::ifstream meta_in(meta_path);
        std::string line_buffer;
        while (std::getline(meta_in, line_buffer)) {
            if (line_buffer.find("message: ") != std::string::npos && line_buffer.find("message: ") == 0) {
                commit_msg = line_buffer.substr(9);
            } else if (line_buffer.find("parent: ") != std::string::npos && line_buffer.find("parent: ") == 0) {
                parent_hash = line_buffer.substr(8);
            }
        }
        if (oneline) {
            std::cout << current_node << " " << commit_msg << "\n";
        } else {
            std::cout << "Commit: " << current_node << "\n";
            std::cout << "Message: " << commit_msg << "\n";
            std::cout << "\n";
        }
        if (parent_hash != "root") {
            traversal_stack.push_back(parent_hash);
        }
    }
    return 0;
}