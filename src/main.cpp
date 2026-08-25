#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <unordered_set>
#include <unistd.h>
#include <sys/wait.h>

using namespace std;

// Builtin registry for easy lookup
const unordered_set<string> BUILTINS = {"echo", "exit", "type", "pwd", "cd"};

// Parses the PATH environment variable into discrete directory paths
vector<string> get_path_directories()
{
    vector<string> paths;
    const char *path_env = getenv("PATH");

    if (path_env != nullptr)
    {
        stringstream ss(path_env);
        string ind_path;
        while (getline(ss, ind_path, ':'))
        {
            paths.push_back(ind_path);
        }
    }
    return paths;
}

// Searches for an executable within PATH directories
string find_in_path(const string &command, const vector<string> &paths)
{
    for (const string &path : paths)
    {
        string filepath = path + '/' + command;
        if (access(filepath.c_str(), X_OK) == 0)
        {
            return filepath;
        }
    }
    return "";
}

// Handlers for shell builtins
void handle_echo(const string &input)
{
    cout << input.substr(5) << "\n";
}

// Handle type command
void handle_type(const string &input, const vector<string> &paths)
{
    string command = input.substr(5);

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

// Handle externals
void handle_externals(const string &input, const vector<string> &paths)
{
    // seperate by space
    string command;
    vector<string> args;

    size_t space_pos = input.find(' ');

    if (space_pos != string::npos)
    {
        command = input.substr(0, space_pos);

        // args into vector
        string raw_args = input.substr(space_pos + 1);
        stringstream ss(raw_args);
        string arg;

        while (ss >> arg)
        {
            args.push_back(arg);
        }
    }
    else
    {
        command = input;
    }

    // Determine if command is executable
    string filepath = find_in_path(command, paths);
    if (filepath == "")
    {
        cout << command << ": not found\n";
    }
    else
    {
        // Create a C++ vector to store C-style char pointers
        vector<char *> argv;

        // 1. argv[0] must be the path/command name
        argv.push_back(const_cast<char *>(command.c_str()));

        // 2. argv[1..n] are the arguments
        for (const string &arg : args)
        {
            argv.push_back(const_cast<char *>(arg.c_str()));
        }

        // 3. Must end with a NULL sentinel pointer
        argv.push_back(nullptr);

        // Call fork
        pid_t pid = fork();

        // Handle Errors
        if (pid < 0)
        {
            perror("fork failed!!");
            return;
        }

        // Handle Child Process (pid == 0)
        if (pid == 0)
        {
            execv(filepath.c_str(), argv.data());

            // This only executes if execv FAILED!!
            perror("execv failed");
            exit(EXIT_FAILURE);
        }

        // 5. PARENT PROCESS (pid > 0)
        int status;
        // waitpid pauses parent until child with this specific PID finishes
        if (waitpid(pid, &status, 0) == -1)
        {
            perror("waitpid failed");
        }
    }
}

// pwd
void handle_pwd()
{
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)))
    {
        cout << cwd << endl;
    }
}

// cd
void handle_cd(const string &path)
{
}

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

        if (input == "exit")
        {
            break;
        }
        else if (input == "pwd")
        {
            handle_pwd();
            continue;
        }
        else if (input.substr(0, 3) == "cd ")
        {
            string path = input.substr(3);

            if (chdir(path.c_str()) != 0)
            {
                perror("cd");
            }

            continue;
        }

        // Fetch PATH per iteration (accounts for runtime updates)
        vector<string> paths = get_path_directories();

        if (input.substr(0, 5) == "echo ")
        {
            handle_echo(input);
        }
        else if (input.substr(0, 5) == "type ")
        {
            handle_type(input, paths);
        }
        else
        {
            handle_externals(input, paths);
        }
    }

    return 0;
}