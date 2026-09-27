#include "builtins.hpp"
#include "env.hpp"
#include "externals.hpp"
#include "parser.hpp"

#include <iostream>
#include <cstring>
#include <readline/readline.h>

using namespace std;

char* generator(const char* text, int state) {
    static int index;

    if (state == 0) {
        index = 0;
    }

    const char* commands[] = {"echo", "exit"};

    while (index < std::size(commands)) {
        const char* command = commands[index++];

        if (std::string(command).starts_with(text)) {
            return strdup(command);
        }
    }

    return nullptr;
};
char** completer(const char*text, int start, int end){

    return  rl_completion_matches(text, generator);
};





int main()
{
    cout << unitbuf;
    cerr << unitbuf;

    rl_attempted_completion_function = completer;

    while (true)
    {
        
        char* input = readline("$ ");
        if (input == nullptr)
        {
            break;
        }

        Command parsed = parse_input(input);
        Command cmd = parse_command(parsed);
        free(input);

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