#pragma once

#include <string>
#include <vector>

struct Command
{
    std::vector<std::string> args; // clean command and arguments
    std::string stdout_file = "";  // target file for redirection
    bool redirect_stdout = false;  // flag indicating if redirection exists
    bool redirect_stderr = false; //flag if error redirection 2>
};
