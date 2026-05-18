#include "CLI.hpp"
#include "commands/version_command.hpp"
#include "commands/init_command.hpp"
#include "commands/cat_file_command.hpp"
#include "commands/write_tree_command.hpp"
#include "commands/read_tree_command.hpp"
#include "commands/commit_command.hpp"
#include "commands/log_command.hpp" // NEW

#include <memory>
#include <vector>

int main(int argc, char **argv) {
    CLI::App app{"DAGit"};
    app.require_subcommand(1);

    std::vector<std::unique_ptr<ICommand>> commands;

    commands.push_back(std::make_unique<VersionCommand>());
    commands.push_back(std::make_unique<InitCommand>());
    commands.push_back(std::make_unique<CatFileCommand>());
    commands.push_back(std::make_unique<WriteTreeCommand>());
    commands.push_back(std::make_unique<ReadTreeCommand>());
    commands.push_back(std::make_unique<CommitCommand>());
    commands.push_back(std::make_unique<LogCommand>()); // NEW

    for (auto &cmd : commands) {
        cmd->setup(app);
    }

    CLI11_PARSE(app, argc, argv);
    return 0;
}