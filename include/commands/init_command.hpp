#pragma once

#include "icommand.hpp"
#include "core/repository.hpp"
#include <iostream>

class InitCommand : public ICommand {
public:
    void setup(CLI::App &app) override {
        // Create subcommand: dagit init
        auto *sub = app.add_subcommand(
            "init", 
            "Initialize an empty DAGit repository"
        );

        // Action performed when command runs
        sub->callback([]() {
            bool success = core::Repository::init();
            if (!success) {
                std::cerr << "Failed to initialize repository.\n";
            }
        });
    }
};