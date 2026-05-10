#pragma once

#include <filesystem>
#include <optional>
#include <string>

namespace core {

class Repository {
public:
    static bool init(const std::string &path = ".");
    
    // NEW: Recursively find the root of the .dagit repository
    static std::optional<std::filesystem::path> find_repo_root();
};

} // namespace core