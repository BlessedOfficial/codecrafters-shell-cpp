#include "builtins.hpp"
#include "env.hpp"

#include <iostream>
#include <unordered_set>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

using namespace std;

const unordered_set<string> BUILTINS = {"echo", "exit", "type", "pwd", "cd"};

void handle_echo(const Command &cmd)
{
    if (cmd.redirect_stdout)
    {
        pid_t pid = fork();

        if (pid < 0)
        {
            perror("fork failed");
            return;
        }

        if (pid == 0) // CHILD PROCESS
        {
            int fd = open(cmd.stdout_file.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (fd < 0)
            {
                perror("open failed");
                exit(EXIT_FAILURE);
            }
            dup2(fd, STDOUT_FILENO); // STDOUT_FILENO is 1
            close(fd);

            // Print arguments to redirected stdout
            for (size_t i = 1; i < cmd.args.size(); ++i)
            {
                cout << cmd.args[i];
                if (i + 1 < cmd.args.size())
                {
                    cout << " ";
                }
            }
            cout << '\n';

            exit(EXIT_SUCCESS); // Terminate child execution cleanly
        }
        else // PARENT PROCESS
        {
            int status;
            waitpid(pid, &status, 0); // Wait for child process to finish
        }
    }
    else // NORMAL ECHO
    {
        for (size_t i = 1; i < cmd.args.size(); ++i)
        {
            cout << cmd.args[i];
            if (i + 1 < cmd.args.size())
            {
                cout << " ";
            }
        }
        cout << '\n'; // Added missing newline
    }
}

void handle_type(const Command &cmd, const vector<string> &paths)
{
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
