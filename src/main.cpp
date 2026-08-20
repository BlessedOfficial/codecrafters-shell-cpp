#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <sys/stat.h>

using namespace std;

inline bool exists_test(const std::string& name);

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
                if (exists_test(filepath))
                {
                  cout << command + " is " + filepath <<endl;
                 
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

inline bool exists_test (const std::string& name) {
  struct stat buffer;   
  return (stat (name.c_str(), &buffer) == 0); 
}