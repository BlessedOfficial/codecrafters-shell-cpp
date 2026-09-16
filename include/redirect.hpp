#pragma once

#include "command.hpp"

void redirect_stdout(const Command& cmd);

void redirect_stderr(const Command& cmd);

void redirect_stderr_append(const Command& cmd);

void redirect_stdout_append(const Command& cmd);