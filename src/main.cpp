#include "builtins.hpp"
#include "env.hpp"
#include "externals.hpp"
#include "parser.hpp"

#include <iostream>

using namespace std;

int main()
{
    cout << unitbuf;
    cerr << unitbuf;

    while (true)
    {
        cout << "$ ";

        string input;
        if (!getline(cin, input))
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

    return 0;
}
