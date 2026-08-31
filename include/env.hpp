#pragma once

#include <string>
#include <vector>

std::vector<std::string> get_path_directories();
std::string get_home_env_var();
std::string find_in_path(const std::string &command, const std::vector<std::string> &paths);
