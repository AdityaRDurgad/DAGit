#pragma once
#include "icommand.hpp"
#include <iostream>

class VersionCommand : public ICommand {
public:
    void setup(CLI::App &app) override {
        // Create subcommand: dagit version
        auto *sub = app.add_subcommand(
            "version",
            "Display DAGit version"
        );

        // Action performed when command runs
        sub->callback([]() {
            std::cout << "DAGit version 1.0" << std::endl;
        });
    }
};