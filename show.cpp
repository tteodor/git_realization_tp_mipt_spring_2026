#include <iostream>
#include <fstream>
#include <filesystem>
#include <string>

namespace fs = std::filesystem;

std::string get_commit_id2(const std::string& target) {
    fs::path branch_file = fs::path(".gitpta/branches") / target;
    std::string target_commit_id;
    if (fs::exists(branch_file)) {
        std::ifstream in(branch_file);
        in >> target_commit_id;
        return target_commit_id;
    }
    return target;
}

void reformat(std::string& str, const std::string& from, const std::string& to) {
    size_t i = str.find(from, 0);
    while (i != std::string::npos) {
        str.replace(i, from.length(), to);
        i += to.length();
        i = str.find(from, i);
    }
}

int command_show(int argc, char** argv) {
    std::string target = "";
    std::string format = "commit: %H\nmessage: %s\nparent: %P";
    for (int i = 2; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg.find("--format=") == 0) {
            format = arg.substr(9);
        } else {
            target = arg;
        }
    }

    fs::path gitpta_dir = ".gitpta";
    if (!fs::exists(gitpta_dir)) {
        std::cerr << "error: not a gitpta repository\n";
        return 10;
    }
    if (target.empty()) {
        fs::path head_file = gitpta_dir / "head";
        std::ifstream in(head_file);
        in >> target;
    }
    std::string commit_id = get_commit_id2(target);
    if (commit_id == "root") {
        std::cout << "no commits yet\n";
        return 0;
    }

    fs::path commit_dir = gitpta_dir / "commits" / commit_id;
    if (!fs::exists(commit_dir)) {
        std::cerr << "error: unknown commit "  << commit_id << "\n";
        return 11;
    }

    std::string message = "no message";
    std::string parent = "root";
    fs::path meta_file = commit_dir / ".meta";
    if (fs::exists(meta_file)) {
        std::ifstream in(meta_file);
        std::string line;
        while (std::getline(in, line)) {
            if (line.find("message: ") == 0) {
                message = line.substr(9);
            } else if (line.find("parent: ") == 0) {
                parent = line.substr(8);
            }
        }
    }

    std::string output = format;
    reformat(output, "%H", commit_id);
    reformat(output, "%s", message);
    reformat(output, "%P", parent);
    reformat(output, "\\n", "\n");
    std::cout << output << "\n";
    std::cout << "files in this commit:\n";
    for (const auto& entry : fs::directory_iterator(commit_dir)) {
        if (entry.path().filename() != ".meta") {
            std::cout << entry.path().filename().string() << "\n";
            std::ifstream in(entry);
            std::string line;
            while (std::getline(in, line)) {
                std::cout << line << "\n";
            }
        }
    }
    return 0;
}

