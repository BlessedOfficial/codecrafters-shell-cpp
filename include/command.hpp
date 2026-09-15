#pragma once

#include <string>
#include <vector>

struct Command
{
    std::vector<std::string> args; // clean command and arguments
    std::string stdout_file = "";  // target file for redirection
    std::string stderr_file = ""; // target file redirected errors
    bool has_redirect_stdout = false;  // flag indicating if redirection exists
    bool has_redirect_stderr = false; //flag if error redirection 2>
};
