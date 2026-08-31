#include "env.hpp"

#include <cstdlib>
#include <sstream>
#include <unistd.h>

using namespace std;

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
