#include "command.hpp"

#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>


void redirect_stdout(const Command& cmd){
    int fd = open(cmd.stdout_file.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (fd < 0)
            {
                perror("open failed");
                exit(EXIT_FAILURE);
            }
            dup2(fd, STDOUT_FILENO); // STDOUT_FILENO is 1
            close(fd);
}

void redirect_stderr(const Command& cmd){
      int fd = open(cmd.stderr_file.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (fd < 0)
            {
                perror("open failed");
                exit(EXIT_FAILURE);
            }
            dup2(fd, STDERR_FILENO); // STDERR_FILENO is 2
            close(fd);
}

void redirect_stderr_append(const Command& cmd){
      int fd = open(cmd.stderr_file.c_str(), O_WRONLY | O_CREAT | O_APPEND, 0644);
            if (fd < 0)
            {
                perror("open failed");
                exit(EXIT_FAILURE);
            }
            dup2(fd, STDERR_FILENO); // STDERR_FILENO is 2
            close(fd);
}

void redirect_stdout_append(const Command& cmd){
    int fd = open(cmd.stdout_file.c_str(), O_WRONLY | O_CREAT | O_APPEND, 0644);
            if (fd < 0)
            {
                perror("open failed");
                exit(EXIT_FAILURE);
            }
            dup2(fd, STDOUT_FILENO); // STDOUT_FILENO is 1
            close(fd);
}