#include "commands/read_tree_command.hpp"
#include "core/tree.hpp"
#include <iostream>

void ReadTreeCommand::setup(CLI::App &app) {
    auto *sub = app.add_subcommand("read-tree", "Read tree information into the working directory");
    sub->add_option("tree", tree_oid, "The tree object ID")->required();

    sub->callback([this]() {
        try {
            core::Tree::read_tree(tree_oid);
            // Be silent on success, like real Git
        } catch (const std::exception& e) {
            std::cerr << e.what() << "\n";
        }
    });
}