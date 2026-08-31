#pragma once

#include "command.hpp"

#include <vector>

void handle_echo(const Command &cmd);
void handle_type(const Command &cmd, const std::vector<std::string> &paths);
void handle_pwd();
void handle_cd(const Command &cmd);
