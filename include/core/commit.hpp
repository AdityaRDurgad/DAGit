#pragma once

#include <string>
#include <vector>
#include <set>
#include <unordered_set>

namespace core {

struct CommitObject {
    std::string tree_oid;
    std::vector<std::string> parents;
    std::string message;
};

class Commit {
public:
    static std::string create_commit(const std::string& message);
    static CommitObject get_commit(const std::string& oid);
    static std::unordered_set<std::string> iter_commits_and_parents(const std::set<std::string>& oids);
};

} // namespace core