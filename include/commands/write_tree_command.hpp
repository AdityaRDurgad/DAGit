#pragma once
#include "icommand.hpp"

class WriteTreeCommand : public ICommand {
public:
    void setup(CLI::App &app) override;
};