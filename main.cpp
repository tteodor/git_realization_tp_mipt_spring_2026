#include <iostream>
#include <string>
#include <vector>


// #include <init.hh>
// #include <add.hh>
// #include <commit.hh>
// #include <log.hh>
// #include <status.hh>
// #include <show.hh>
// #include <pure_merge.hh>

int command_init(int args, char** argv);
int command_add(int args, char** argv);
int command_commit(int args, char** argv);
int command_status(int args, char** argv);
int command_log(int args, char** argv);
int command_show(int args, char** argv);
int command_pure_merge(int args, char** argv);

int main(int argc, char* argv[]) {
    std::vector<std::string> args(argv, argv + argc);
    if (args.size() < 2) {
        std::cerr << "You need write at least 1 argument \n";
        return 1;
    }
    std::string command = argv[1];
    if (command == "init") {
        command_init(argc, argv);
    } else if (command == "add") {
        command_add(argc, argv);
    } else if (command == "commit") {
        command_commit(argc, argv);
    } else if (command == "status") {
        command_status(argc, argv);
    } else if (command == "log") {
        command_log(argc, argv);
    } else if (command == "show") {
        command_show(argc, argv);
    } else if (command == "pure_merge") {
        command_pure_merge(argc, argv);
    } else {
        std::cerr << "Unknown command: " << command << "\n";
        return 1;
    }
    return 0;
}
