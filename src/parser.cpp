#include "parser.hpp"

using namespace std;

Command parse_input(const string &input)
{
    Command cmd;
    string curr_string = "";

    bool is_inside_single_quotes = false;
    bool is_inside_double_quotes = false;
    bool in_token = false;

    for (size_t i = 0; i < input.length(); ++i)
    {
        char c = input[i];

        // =========================================================
        // BACKSLASH HANDLING
        // =========================================================
        if (c == '\\')
        {
            // -----------------------------------------------------
            // Backslash inside single quotes:
            // It has no special meaning.
            // -----------------------------------------------------
            if (is_inside_single_quotes)
            {
                curr_string += c;
                in_token = true;
                continue;
            }

            // -----------------------------------------------------
            // Backslash inside double quotes:
            // It only escapes:
            //     "
            //     \
            //     $
            //     `
            // -----------------------------------------------------
            if (is_inside_double_quotes)
            {
                ++i;

                if (i < input.length())
                {
                    char next = input[i];

                    if (next == '"' ||
                        next == '\\' ||
                        next == '$' ||
                        next == '`')
                    {
                        // Escaped special character:
                        // remove the backslash
                        curr_string += next;
                    }
                    else
                    {
                        // Backslash has no special meaning for
                        // this character, so preserve it.
                        curr_string += '\\';
                        curr_string += next;
                    }

                    in_token = true;
                }

                continue;
            }

            // -----------------------------------------------------
            // Backslash outside quotes:
            // It escapes ANY character.
            // -----------------------------------------------------
            ++i;

            if (i < input.length())
            {
                curr_string += input[i];
                in_token = true;
            }

            continue;
        }

        // =========================================================
        // DOUBLE QUOTES
        // =========================================================
        if (c == '"' && !is_inside_single_quotes)
        {
            is_inside_double_quotes = !is_inside_double_quotes;
            in_token = true;
            continue;
        }

        // =========================================================
        // SINGLE QUOTES
        // =========================================================
        if (c == '\'' && !is_inside_double_quotes)
        {
            is_inside_single_quotes = !is_inside_single_quotes;
            in_token = true;
            continue;
        }

        // =========================================================
        // SPACES / TABS OUTSIDE QUOTES
        // =========================================================
        if ((c == ' ' || c == '\t') &&
            !is_inside_single_quotes &&
            !is_inside_double_quotes)
        {
            if (in_token)
            {
                cmd.args.push_back(curr_string);
                curr_string = "";
                in_token = false;
            }

            continue;
        }

        // =========================================================
        // NORMAL CHARACTER
        // =========================================================
        curr_string += c;
        in_token = true;
    }

    // =============================================================
    // PUSH FINAL TOKEN
    // =============================================================
    if (in_token)
    {
        cmd.args.push_back(curr_string);
    }

    return cmd;
}

Command parse_command(const Command &parsed)
{
    Command cmd;

    for (size_t i = 0; i < parsed.args.size(); ++i)
    {
        // Check if redirection operator exists (>)
        if (parsed.args[i] == ">" || parsed.args[i] == "1>")
        {
            // Verify filename exists
            if (i + 1 < parsed.args.size())
            {
                cmd.redirect_stdout = true;
                cmd.stdout_file = parsed.args[i + 1];
                ++i;
            }
            else
            {
                // error code
            }
        }
        else
        {
            cmd.args.push_back(parsed.args[i]);
        }
    }

    return cmd;
}
