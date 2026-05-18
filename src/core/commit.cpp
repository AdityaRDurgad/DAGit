#include "core/commit.hpp"
#include "core/repository.hpp"
#include "core/tree.hpp"
#include "core/object_store.hpp"
#include <filesystem>
#include <fstream>
#include <sstream>
#include <deque>
#include <stdexcept>
namespace fs = std::filesystem;
namespace core {

std::string Commit::get_head() {
    auto repo_root = Repository::find_repo_root();
    if (!repo_root) return "";

    fs::path head_path = *repo_root / ".dagit" / "HEAD";
    if (!fs::exists(head_path)) return "";

    std::ifstream in(head_path);
    std::string oid;
    in >> oid; // >> operator safely extracts the string and ignores trailing newlines
    
    return oid;
}

void Commit::set_head(const std::string& oid) {
    auto repo_root = Repository::find_repo_root();
    if (!repo_root) throw std::runtime_error("Fatal: Not a dagit repository.");

    fs::path head_path = *repo_root / ".dagit" / "HEAD";
    std::ofstream out(head_path);
    out << oid << "\n";
}

// --- UPDATED COMMIT CREATION ---

std::string Commit::create_commit(const std::string& message) {
    auto repo_root = Repository::find_repo_root();
    if (!repo_root) throw std::runtime_error("Fatal: Not a dagit repository.");

    // 1. Snapshot the working directory
    std::string tree_oid = Tree::write_tree(*repo_root);

    // 2. Fetch current HEAD to act as parent
    std::string parent_oid = get_head();

    // 3. Construct commit content with parent linkage
    std::string commit_data = "tree " + tree_oid + "\n";
    if (!parent_oid.empty()) {
        commit_data += "parent " + parent_oid + "\n";
    }
    commit_data += "\n" + message + "\n";

    // 4. Hash and store the commit object
    std::string new_commit_oid = ObjectStore::hash_object("commit", commit_data);

    // 5. Update HEAD to point to this new commit
    set_head(new_commit_oid);

    return new_commit_oid;
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