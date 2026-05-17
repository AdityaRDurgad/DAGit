#pragma once
#include "icommand.hpp"
#include <string>

class CatFileCommand : public ICommand {
private:
    std::string type;
    std::string oid;
public:
    void setup(CLI::App &app) override;
};