#pragma once
#include "icommand.hpp"
#include <string>

class ReadTreeCommand : public ICommand {
private:
    std::string tree_oid;
public:
    void setup(CLI::App &app) override;
};