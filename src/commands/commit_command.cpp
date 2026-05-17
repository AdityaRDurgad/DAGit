#include "commands/commit_command.hpp"
#include "core/commit.hpp"
#include <iostream>

void CommitCommand::setup(CLI::App &app) {
    auto *sub = app.add_subcommand("commit", "Record changes to the repository");
    
    // Adding the standard -m flag for the commit message
    sub->add_option("-m,--message", message, "Commit message")->required();

    sub->callback([this]() {
        try {
            std::string commit_oid = core::Commit::create_commit(message);
            std::cout << "[master (root-commit) " << commit_oid.substr(0, 7) << "] " << message << "\n";
        } catch (const std::exception& e) {
            std::cerr << e.what() << "\n";
        }
    });
}