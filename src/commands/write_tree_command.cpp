#include "commands/write_tree_command.hpp"
#include "core/repository.hpp"
#include "core/tree.hpp"
#include <iostream>

void WriteTreeCommand::setup(CLI::App &app) {
    auto *sub = app.add_subcommand("write-tree", "Create a tree object from the workspace");

    sub->callback([]() {
        try {
            auto repo_root = core::Repository::find_repo_root();
            if (!repo_root) {
                throw std::runtime_error("Fatal: Not a dagit repository.");
            }

            std::string oid = core::Tree::write_tree(*repo_root);
            std::cout << oid << "\n";
        } catch (const std::exception& e) {
            std::cerr << e.what() << "\n";
        }
    });
}