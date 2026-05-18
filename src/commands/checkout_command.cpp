#include "commands/checkout_command.hpp"
#include "core/repository.hpp"
#include "core/commit.hpp"
#include "core/tree.hpp"
#include "core/object_store.hpp"
#include <iostream>
#include <stdexcept>

void CheckoutCommand::setup(CLI::App &app) {
    auto *sub = app.add_subcommand("checkout", "Restore working tree files to a specific commit");
    
    // Accept the required commit OID
    sub->add_option("commit", commit_oid, "The commit OID to checkout")->required();

    sub->callback([this]() {
        try {
            // 1. Validate Repository
            auto repo_root = core::Repository::find_repo_root();
            if (!repo_root) {
                throw std::runtime_error("Fatal: Not a dagit repository.");
            }

            // 2. Validate Commit OID Exists
            if (!core::ObjectStore::object_exists(commit_oid)) {
                throw std::runtime_error("Fatal: invalid oid " + commit_oid);
            }

            // 3. Read Commit Object
            // (Note: get_commit will automatically validate that the object is actually a commit)
            core::CommitObject commit = core::Commit::get_commit(commit_oid);

            // 4. Restore Repository Snapshot
            core::Tree::read_tree(commit.tree_oid);

            // 5. Move HEAD
            core::Commit::set_head(commit_oid);

            // 6. Print Checkout Status (using short 7-character OID like Git)
            std::cout << "HEAD is now at " << commit_oid.substr(0, 7) << "\n";

        } catch (const std::exception& e) {
            std::cerr << e.what() << "\n";
        }
    });
}