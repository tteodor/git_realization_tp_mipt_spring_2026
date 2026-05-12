#include "ignore.hh"
#include <fstream>
#include <algorithm>

namespace fs = std::filesystem;

bool is_ignored(const fs::path& path) {
  fs::path ignore_file = ".gitpta_ignore";
  if (!fs::exists(ignore_file)) {
    return false;
  }

  std::ifstream in(ignore_file);
  std::string line;
  std::string path_str = path.string();
  while (std::getline(in, line)) {
    if (line.empty() || line[0] == '#') {
      continue;
    }
    if (path_str.find(line) != std::string::npos) {
      return true;
    }
  }
  return false;
}

