#include "core/commit.hpp"
#include "core/repository.hpp"
#include "core/tree.hpp"
#include "core/object_store.hpp"

#include <sstream>
#include <deque>
#include <stdexcept>

namespace core {

std::string Commit::create_commit(const std::string& message) {
    auto repo_root = Repository::find_repo_root();
    if (!repo_root) throw std::runtime_error("Fatal: Not a dagit repository.");

    // 1. Snapshot the working directory
    std::string tree_oid = Tree::write_tree(*repo_root);

    // 2. Construct commit content (simplistic approach for now)
    std::string commit_data = "tree " + tree_oid + "\n";
    commit_data += "\n" + message + "\n";

    // 3. Hash and store
    return ObjectStore::hash_object("commit", commit_data);
}

CommitObject Commit::get_commit(const std::string& oid) {
    std::string content = ObjectStore::get_object(oid, "commit");
    std::istringstream iss(content);
    std::string line;
    
    CommitObject commit;
    
    // Parse metadata headers
    while (std::getline(iss, line) && !line.empty()) {
        std::istringstream line_stream(line);
        std::string key, val;
        line_stream >> key >> val;
        
        if (key == "tree") {
            commit.tree_oid = val;
        } else if (key == "parent") {
            commit.parents.push_back(val);
        }
    }
    
    // The rest is the commit message
    std::ostringstream message_stream;
    while (std::getline(iss, line)) {
        message_stream << line << "\n";
    }
    commit.message = message_stream.str();
    
    // Trim trailing newline for clean output
    if (!commit.message.empty() && commit.message.back() == '\n') {
        commit.message.pop_back();
    }
    
    return commit;
}

std::unordered_set<std::string> Commit::iter_commits_and_parents(const std::set<std::string>& oids) {
    std::unordered_set<std::string> visited;
    std::deque<std::string> queue(oids.begin(), oids.end());

    while (!queue.empty()) {
        std::string current_oid = queue.front();
        queue.pop_front();

        // Prevent infinite loops in complex histories
        if (visited.count(current_oid)) continue;
        visited.insert(current_oid);

        CommitObject commit = get_commit(current_oid);
        
        // Queue up the parents for traversal
        for (const auto& parent_oid : commit.parents) {
            if (!visited.count(parent_oid)) {
                queue.push_back(parent_oid);
            }
        }
    }

    return visited;
}

} // namespace core