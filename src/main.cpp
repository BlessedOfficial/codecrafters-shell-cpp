#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <unordered_set>
#include <unistd.h>

using namespace std;

// Builtin registry for easy lookup
const unordered_set<string> BUILTINS = {"echo", "exit", "type"};

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

void handle_type(const string &input, const vector<string> &paths)
{
    string command = input.substr(5);

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
            cout << input << ": command not found\n";
        }
    }

    return 0;
}