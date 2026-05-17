#include "core/repository.hpp"
#include <filesystem>
#include <iostream>

namespace fs = std::filesystem;

namespace core {

bool Repository::init(const std::string &path) {
    // 1. NEW: Check if we are already inside a repository anywhere in the parent chain
    auto existing_repo = find_repo_root();
    if (existing_repo) {
        std::cerr << "Fatal: Cannot initialize a new DAGit repository.\n";
        std::cerr << "You are already inside an existing repository at: " << existing_repo->string() << "\n";
        return false;
    }

    fs::path repo_path = fs::path(path) / ".dagit";

    // 2. Existing logic: Create the folders
    if (fs::exists(repo_path)) {
        std::cout << "DAGit repository already exists in " << repo_path.string() << "\n";
        return true;
    }

    try {
        fs::create_directories(repo_path / "objects");
        fs::create_directories(repo_path / "refs" / "heads");

        std::cout << "Initialized empty DAGit repository in " << fs::absolute(repo_path).string() << "\n";
        return true;
    } catch (const std::exception &e) {
        std::cerr << "Failed to initialize repository: " << e.what() << "\n";
        return false;
    }
}

// ... [Keep your existing find_repo_root() function down here] ...

std::optional<fs::path> Repository::find_repo_root() {
    fs::path current = fs::current_path();

    while (true) {
        if (fs::exists(current / ".dagit")) {
            return current;
        }
        
        if (current == current.root_path()) {
            break;
        }
        
        current = current.parent_path();
    }

    return std::nullopt;
}

} // namespace core