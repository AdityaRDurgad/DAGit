#pragma once
#include "icommand.hpp"
#include <string>

class CheckoutCommand : public ICommand {
private:
    std::string commit_oid;
public:
    void setup(CLI::App &app) override;
};