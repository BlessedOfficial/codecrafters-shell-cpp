#include "builtins.hpp"
#include "env.hpp"
#include "redirect.hpp"

#include <iostream>
#include <unordered_set>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

using namespace std;

const unordered_set<string> BUILTINS = {"echo", "exit", "type", "pwd", "cd"};

void handle_echo(const Command &cmd)
{
    if (cmd.has_redirect_stdout || cmd.has_redirect_stderr)
    {
        pid_t pid = fork();

        if (pid < 0)
        {
            perror("fork failed");
            return;
        }

        if (pid == 0) // CHILD PROCESS
        {
            if (cmd.has_redirect_stdout)
                redirect_stdout(cmd);

            if (cmd.has_redirect_stderr)
                redirect_stderr(cmd);
        }
        else // PARENT PROCESS
        {
            int status;
            waitpid(pid, &status, 0); // Wait for child process to finish
            return;
        }
    }

    for (size_t i = 1; i < cmd.args.size(); ++i)
    {
        cout << cmd.args[i];
        if (i + 1 < cmd.args.size())
        {
            cout << " ";
        }
    }
    cout << '\n'; // Added missing newline

    if (cmd.has_redirect_stdout || cmd.has_redirect_stderr)
    {
        exit(EXIT_SUCCESS);
    }
}

void handle_type(const Command &cmd, const vector<string> &paths)
{
    if (cmd.args.size() < 2)
        return;

    if (cmd.has_redirect_stdout || cmd.has_redirect_stderr)
    {
        pid_t pid = fork();

        if (pid < 0)
        {
            perror("fork failed");
            return;
        }

        if (pid == 0) // CHILD PROCESS
        {
            if (cmd.has_redirect_stdout)
                cmd.redirect_stdout(cmd);

            if (cmd.has_redirect_stderr)
                cmd.redirect_stderr(cmd);
        }
        else // PARENT PROCESS
        {
            int status;
            waitpid(pid, &status, 0); // Wait for child process to finish
            return;
        }
    }

    string command = cmd.args[1];

    // O(1) lookup Optimisation
    if (BUILTINS.count(command))
    {
        cout << command << " is a shell builtin\n";
    }
    else
    {
        string filepath = find_in_path(command, paths);
        if (!filepath.empty())
        {
            cout << command << " is " << filepath << "\n";
        }
        else
        {
            cout << command << ": not found\n";
        }
    }
    if (cmd.has_redirect_stderr || cmd.has_redirect_stdout)
    {
        exit(EXIT_SUCCESS);
    }
}

void handle_pwd()
{
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)))
    {
        cout << cwd << endl;
    }
}

void handle_cd(const Command &cmd)
{
    string path = cmd.args.size() > 1 ? cmd.args[1] : get_home_env_var();

    if (path == "~")
    {
        path = get_home_env_var();
    }

    if (chdir(path.c_str()) != 0)
    {
        cerr << "cd: " << path << ": No such file or directory" << endl;
    }
}
