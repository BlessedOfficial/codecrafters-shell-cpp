#include "builtins.hpp"
#include "env.hpp"
#include "externals.hpp"
#include "parser.hpp"

#include <iostream>
#include <readline/readline.h>

using namespace std;

int main()
{
    cout << unitbuf;
    cerr << unitbuf;

    while (true)
    {
        char* input = readline("$ ");
        if (input == nullptr)
        {
            break;
        }

        Command parsed = parse_input(input);
        Command cmd = parse_command(parsed);

        if (cmd.args.empty())
        {
            continue;
        }

        string command = cmd.args[0];

        if (command == "exit")
        {
            break;
        }
        else if (command == "pwd")
        {
            handle_pwd();
            continue;
        }
        else if (command == "cd")
        {
            handle_cd(cmd);
            continue;
        }

        vector<string> paths = get_path_directories();

        if (command == "echo")
        {
            handle_echo(cmd);
        }
        else if (command == "type")
        {
            handle_type(cmd, paths);
        }
        else
        {
            handle_externals(cmd, paths);
        }
    }

    free(input);

    return 0;
}
