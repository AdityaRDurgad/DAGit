#include "core/repository.hpp"
#include <filesystem>
#include <iostream>
#include <system_error>

namespace fs = std::filesystem;

namespace core {

bool Repository::init(const std::string &path) {
    try {
        fs::path repo_path = fs::path(path) / ".dagit";

        // 1. Check if .dagit already exists
        if (fs::exists(repo_path)) {
            std::cout << "DAGit repository already exists in " << repo_path.string() << "\n";
            return false; 
        }

        // 2. Create the hidden directory and objects sub-directory
        fs::create_directory(repo_path);
        fs::create_directory(repo_path / "objects");

        std::cout << "Initialized empty DAGit repository in " << repo_path.string() << "\n";
        return true;

    } catch (const fs::filesystem_error& e) {
        // 3. Proper filesystem error handling
        std::cerr << "Filesystem error during init: " << e.what() << "\n";
        return false;
    } catch (const std::exception& e) {
        std::cerr << "An unexpected error occurred: " << e.what() << "\n";
        return false;
    }
}
std::optional<fs::path> Repository::find_repo_root() {
    fs::path current = fs::current_path();

    while (true) {
        if (fs::exists(current / ".dagit")) {
            return current;
        }
        
        // If we reach the system root directory without finding it
        if (current == current.root_path()) {
            break;
        }
        
        current = current.parent_path();
    }

    return std::nullopt;
}

} // namespace core