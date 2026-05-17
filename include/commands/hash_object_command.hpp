#pragma once
#include "icommand.hpp"
#include <string>

class HashObjectCommand : public ICommand {
private:
    std::string file_path;
public:
    void setup(CLI::App &app) override;
};