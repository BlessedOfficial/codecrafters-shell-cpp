#pragma once

#include "command.hpp"

#include <string>

Command parse_input(const std::string &input);
Command parse_command(const Command &parsed);
