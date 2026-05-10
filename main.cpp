#include "CLI.hpp"
#include "commands/version_command.hpp"
#include "commands/init_command.hpp"

#include <memory>
#include <vector>

int main(int argc, char **argv) {
    
    CLI::App app{"DAGit - A modular version control system"};
    
    // Require the user to type at least one command
    app.require_subcommand(1);

    std::vector<std::unique_ptr<ICommand>> commands;

    // Register all commands
    commands.push_back(std::make_unique<VersionCommand>());
    commands.push_back(std::make_unique<InitCommand>());

    // Execute the setup sequence for each command
    for (auto &cmd : commands) {
        cmd->setup(app);
    }

    // Parse the terminal input
    CLI11_PARSE(app, argc, argv);

    return 0;
}