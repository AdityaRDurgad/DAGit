#include "core/tree.hpp"
#include "core/object_store.hpp"
#include "core/repository.hpp"
#include <map>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>

namespace fs = std::filesystem;

namespace core {

std::string Tree::write_tree(const fs::path& directory) {
    std::vector<std::string> entries;

    for (const auto& entry : fs::directory_iterator(directory)) {
        std::string filename = entry.path().filename().string();

        // 1. Ignore .dagit
        if (filename == ".dagit") continue;

        if (entry.is_directory()) {
            // 2. Recursively hash subtree
            std::string subtree_oid = write_tree(entry.path());
            entries.push_back("tree " + subtree_oid + " " + filename);
        } 
        else if (entry.is_regular_file()) {
            // Hash blob
            std::ifstream file(entry.path(), std::ios::binary);
            std::stringstream buffer;
            buffer << file.rdbuf();
            
            std::string blob_oid = ObjectStore::hash_object("blob", buffer.str());
            entries.push_back("blob " + blob_oid + " " + filename);
        }
    }

    // 3. Sort entries deterministically
    std::sort(entries.begin(), entries.end());

    // 4. Combine into tree data
    std::string tree_data = "";
    for (const auto& line : entries) {
        tree_data += line + "\n";
    }

    // 5. Hash the tree object itself
    return ObjectStore::hash_object("tree", tree_data);
}
std::map<std::string, std::string> Tree::get_tree(const std::string& tree_oid, const std::string& base_path) {
    std::map<std::string, std::string> result;
    
    // 1. Read tree object
    std::string content = ObjectStore::get_object(tree_oid, "tree");
    
    // 2. Parse entries line-by-line
    std::istringstream iss(content);
    std::string line;
    
    while (std::getline(iss, line)) {
        if (line.empty()) continue;
        
        std::istringstream line_stream(line);
        std::string type, oid, name;
        line_stream >> type >> oid >> name;
        
        // 3. Construct full relative path
        std::string full_path = base_path.empty() ? name : base_path + "/" + name;
        
        // 4. Store blob mappings or recursively parse subtrees
        if (type == "blob") {
            result[full_path] = oid;
        } else if (type == "tree") {
            auto subtree_map = get_tree(oid, full_path);
            // Merge subtree results into current map
            result.insert(subtree_map.begin(), subtree_map.end());
        }
    }
    
    return result;
}

void Tree::read_tree(const std::string& tree_oid) {
    auto repo_root = Repository::find_repo_root();
    if (!repo_root) {
        throw std::runtime_error("Fatal: Not a dagit repository.");
    }

    // 1. Get the in-memory mapping of path -> blob_oid
    std::map<std::string, std::string> tree_map = get_tree(tree_oid);

    // 2. Traverse current working directory recursively
    std::vector<fs::path> paths_to_delete;
    auto it = fs::recursive_directory_iterator(*repo_root);
    
    for (; it != fs::recursive_directory_iterator(); ++it) {
        // 3. IGNORE .dagit! If we see it, tell iterator to skip its contents.
        if (it->path().filename() == ".dagit") {
            it.disable_recursion_pending();
            continue;
        }
        paths_to_delete.push_back(it->path());
    }

    // 4. Sort paths by descending depth (longest paths first) to safely delete children before parents
    std::sort(paths_to_delete.begin(), paths_to_delete.end(), [](const fs::path& a, const fs::path& b) {
        return std::distance(a.begin(), a.end()) > std::distance(b.begin(), b.end());
    });

    // 5. Clean current working directory
    for (const auto& p : paths_to_delete) {
        std::error_code ec;
        fs::remove(p, ec); // fs::remove safely deletes empty dirs and files
    }

    // 6. Restore the tree from the object database
    for (const auto& [rel_path, oid] : tree_map) {
        fs::path full_path = *repo_root / rel_path;
        
        // Recreate parent directories if they don't exist
        fs::create_directories(full_path.parent_path());
        
        // Read blob content and write to disk
        std::string content = ObjectStore::get_object(oid, "blob");
        std::ofstream out(full_path, std::ios::binary);
        out.write(content.c_str(), content.size());
    }
}
} // namespace core