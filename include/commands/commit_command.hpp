#pragma once
#include "icommand.hpp"
#include <string>

class CommitCommand : public ICommand {
private:
    std::string message;
public:
    void setup(CLI::App &app) override;
};