#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <sys/stat.h>

using namespace std;



int main()
{
    // Flush after every std::cout / std::cerr
    cout << unitbuf;
    cerr << unitbuf;

    while (true)
    {
        cout << "$ ";

        string input;
        getline(cin, input);

        const char* path = getenv("PATH");
        vector<string> paths;

        if (path != nullptr)
        {
            stringstream ss(path);
            string ind_path;

            while (getline(ss, ind_path, ':'))
            {
                paths.push_back(ind_path);
            }
        }

        // Builtin commands
        if (input == "exit")
        {
            break;
        }
        else if (input.substr(0, 5) == "echo ")
        {
            cout << input.substr(5) << endl;
        }

        // Type
        else if (input.substr(0, 5) == "type ")
        {
            string command = input.substr(5);

            if (command == "echo" ||
                command == "exit" ||
                command == "type")
            {
                cout << command << " is a shell builtin" << endl;
            }
            else
            //Locate executable files
            {
              for (string path : paths)
              {
                string filepath = path + '/' + command;
                if (access(filepath.c_str(), X_OK) == 0)
{
    cout << command << " is " << filepath << endl;
    break;
}
              }

                cout << command << ": not found" << endl;
            }
        }

        // Invalid
        else
        {
            cout << input << ": command not found" << endl;
        }
    }
}

