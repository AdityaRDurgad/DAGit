#pragma once

#include <filesystem>
#include <string>
#include <map>

namespace core {

class Tree {
public:
    static std::string write_tree(const std::filesystem::path& directory);
    
    // NEW: Tree Restoration Functions
    static std::map<std::string, std::string> get_tree(const std::string& tree_oid, const std::string& base_path = "");
    static void read_tree(const std::string& tree_oid);
};

} // namespace core