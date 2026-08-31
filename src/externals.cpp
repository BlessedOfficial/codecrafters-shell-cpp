#include "externals.hpp"
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

            if(cmd.redirect_stdout){
                //Open the file
                int fd = open(cmd.stdout_file.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
                if(fd < 0){
                    perror("open failed");
                    exit(EXIT_FAILURE);
                }


                //Redirect Output 
                dup2(fd, STDOUT_FILENO);

                //Close the extra descriptor
                close(fd);

            }
            execv(filepath.c_str(), argv.data());

            perror("execv failed");
            exit(EXIT_FAILURE);
        }

        int status;
        if (waitpid(pid, &status, 0) == -1)
        {
            perror("waitpid failed");
        }
    }
}
