#pragma once
#include "CLI.hpp"

class ICommand {
public:
    virtual ~ICommand() = default;

    // Every command must register itself into the CLI::App instance
    virtual void setup(CLI::App &app) = 0;
};