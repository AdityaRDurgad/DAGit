#include "commands/cat_file_command.hpp"
#include "core/object_store.hpp"
#include <iostream>

void CatFileCommand::setup(CLI::App &app) {
    auto *sub = app.add_subcommand("cat-file", "Provide content of repository objects");
    sub->add_option("type", type, "Expected type (blob, tree)")->required();
    sub->add_option("object", oid, "The object ID (SHA1)")->required();

    sub->callback([this]() {
        try {
            std::string content = core::ObjectStore::get_object(oid, type);
            std::cout << content; 
        } catch (const std::exception& e) {
            std::cerr << e.what() << "\n";
        }
    });
}