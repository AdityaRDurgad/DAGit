#include "commands/log_command.hpp"
#include "core/commit.hpp"
#include "core/repository.hpp"

#include <iostream>

void LogCommand::setup(CLI::App &app) {
    auto *sub = app.add_subcommand("log", "Show commit logs");
    
    // Optional argument: if provided, starts from this OID instead of HEAD
    sub->add_option("commit", start_oid, "Starting commit OID");

    sub->callback([this]() {
        try {
            auto repo_root = core::Repository::find_repo_root();
            if (!repo_root) {
                throw std::runtime_error("Fatal: Not a dagit repository.");
            }

            // 1. Resolve starting point
            std::string current_oid = start_oid;
            if (current_oid.empty()) {
                current_oid = core::Commit::get_head();
                
                if (current_oid.empty()) {
                    std::cerr << "Fatal: your current branch does not have any commits yet\n";
                    return;
                }
            }

            // 2. Traverse the commit chain backward
            while (!current_oid.empty()) {
                // Read the commit object
                core::CommitObject commit = core::Commit::get_commit(current_oid);
                
                // Print Git-style log output
                std::cout << "\033[33mcommit " << current_oid << "\033[0m\n"; // Yellow text for hash
                std::cout << "\n    " << commit.message << "\n\n";

                // Move backward to the parent commit
                if (!commit.parents.empty()) {
                    current_oid = commit.parents[0]; // Assuming linear history for now
                } else {
                    current_oid = ""; // No parent means we hit the root commit. Stop loop.
                }
            }
        } catch (const std::exception& e) {
            std::cerr << e.what() << "\n";
        }
    });
}