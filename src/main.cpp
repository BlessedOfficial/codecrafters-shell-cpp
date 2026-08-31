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

string get_home_env_var()
{
    const char *home_env = getenv("HOME");
    if (home_env != nullptr)
    {
        return string(home_env);
    }

    return "";
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
void handle_echo(const vector<string> &args)
{
    for (size_t i = 1; i < args.size(); ++i)
    {
        cout << args[i];
        if (i + 1 < args.size())
        {
            cout << " "; 
        }
    }
    cout << '\n';
}


vector<string> parse_input(const string &input)
{
    vector<string> args;
    string curr_string = "";

    bool is_inside_single_quotes = false;
    bool is_inside_double_quotes = false;
    bool in_token = false;

    for (size_t i = 0; i < input.length(); ++i)
    {
        char c = input[i];

        // Handle backslash outside quotes
        if (c == '\\' &&
            !is_inside_single_quotes &&
            !is_inside_double_quotes)
        {
            ++i;

            if (i < input.length())
            {
                curr_string += input[i];
                in_token = true;
            }

            continue;
        }

        // Toggle double quotes
        if (c == '"' && !is_inside_single_quotes)
        {
            is_inside_double_quotes = !is_inside_double_quotes;
            in_token = true;
        }

        //Handle Backslash in double qoutes
        else if( c == '\\' && is_inside_double_quotes ){
            i++;
            if(i < input.length()){
            c = input[i];
            if( (c == '\"' || c == '\\' || c == '\$' || c == '\`')){
                curr_string += input[i];
                //in_token already true
            }else{
                i--;
                c = input[i];
            }
            }


        }

        // Toggle single quotes
        else if (c == '\'' && !is_inside_double_quotes)
        {
            is_inside_single_quotes = !is_inside_single_quotes;
            in_token = true;
        }

        // Handle spaces and tabs
        else if ((c == ' ' || c == '\t') &&
                 !is_inside_single_quotes &&
                 !is_inside_double_quotes)
        {
            if (in_token)
            {
                args.push_back(curr_string);
                curr_string = "";
                in_token = false;
            }
        }

        // Normal character
        else
        {
            curr_string += c;
            in_token = true;
        }
    }

    if (in_token)
    {
        args.push_back(curr_string);
    }

    return args;
}

// Handle type command
void handle_type(const vector<string> &args, const vector<string> &paths)
{
    string command = args[1];

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
void handle_externals(const vector<string> &args, const vector<string> &paths)
{
    string command = args[0];
    vector<string> cmd_args(args.begin() + 1, args.end());

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

// pwd
void handle_pwd()
{
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)))
    {
        cout << cwd << endl;
    }
}

// Handle

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

        vector<string> args = parse_input(input);

        if (args.empty())
        {
            continue;
        }

        string command = args[0];

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
            string path = args.size() > 1 ? args[1] : get_home_env_var();

            if (path == "~")
            {
                path = get_home_env_var();
            }

            if (chdir(path.c_str()) != 0)
            {
                cerr << "cd: " << path << ": No such file or directory" << endl;
            }

            continue;
        }

        vector<string> paths = get_path_directories();

        if (command == "echo")
        {
            handle_echo(args);
        }
        else if (command == "type")
        {
            handle_type(args, paths);
        }
        else
        {
            handle_externals(args, paths);
        }
    }

    return 0;
}
