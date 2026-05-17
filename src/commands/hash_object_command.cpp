#include "commands/hash_object_command.hpp"
#include "core/object_store.hpp"

#include <iostream>
#include <fstream>
#include <sstream>

void HashObjectCommand::setup(CLI::App &app) {
    auto *sub = app.add_subcommand("hash-object", "Compute object ID and create a blob");
    sub->add_option("file", file_path, "File to hash")->required()->check(CLI::ExistingFile);

    sub->callback([this]() {
        try {
            std::ifstream file(file_path, std::ios::binary);
            std::stringstream buffer;
            buffer << file.rdbuf();

            std::string oid = core::ObjectStore::hash_object("blob", buffer.str());
            std::cout << oid << "\n";
        } catch (const std::exception& e) {
            std::cerr << e.what() << "\n";
        }
    });
}