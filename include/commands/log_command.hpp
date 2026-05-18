#pragma once
#include "icommand.hpp"
#include <string>

class LogCommand : public ICommand {
private:
    std::string start_oid;
public:
    void setup(CLI::App &app) override;
};