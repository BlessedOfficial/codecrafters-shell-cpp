#include "externals.hpp"
#include "redirect.hpp"
#include "env.hpp"

#include <cstdlib>
#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

using namespace std;

void handle_externals(const Command &cmd, const vector<string> &paths)
{
    string command = cmd.args[0];
    vector<string> cmd_args(cmd.args.begin() + 1, cmd.args.end());

    string filepath = find_in_path(command, paths);
    if (filepath == "")
    {
        cout << command << ": not found\n";
    }
    else
    {
        vector<char *> argv;

        argv.push_back(const_cast<char *>(command.c_str()));

        for (const string &arg : cmd_args)
        {
            argv.push_back(const_cast<char *>(arg.c_str()));
        }

        argv.push_back(nullptr);

        pid_t pid = fork();

        if (pid < 0)
        {
            perror("fork failed!!");
            return;
        }

        if (pid == 0)
        {

            if (cmd.has_redirect_stdout)
            {
                if (cmd.append)
                {
                    redirect_stdout_append(cmd);
                }
                else
                {
                    redirect_stdout(cmd);
                }
            }

            if (cmd.has_redirect_stderr)
            {
                if (cmd.append)
                {
                    redirect_stderr_append(cmd);
                }
                else
                {
                    redirect_stderr(cmd);
                }
            }

            execv(filepath.c_str(), argv.data());

            perror("execv failed");
            exit(EXIT_FAILURE);
        }
        else
        {
            int status;
            if (waitpid(pid, &status, 0) == -1)
            {
                perror("waitpid failed");
            }
        }
    }
}