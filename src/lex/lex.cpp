#include "lex.hpp"
#include "../util/strutil.hpp"
#include "../util/util.hpp"
#include "../util/syntax.hpp"

#include <iostream>
#include <fstream>
#include <vector>
#include <string>

void lex::parse(std::ifstream &infile, std::ofstream &outfile)
{

    util::u64 line_count = 0;

    outfile << "#include <iostream>\n"
               "#include <cmath>\n\n"
               "int main() {\n";

    std::unordered_map<std::string, util::var_data> variables;

    util::u64 scope = 0;

    bool scope_increased = false;
    bool scope_decreased = true;

    while (infile.peek() != EOF)
    {
        std::string line;
        std::getline(infile, line);
        line_count++;

        util::u64 tabs = 0;

        for (char c : line)
        {
            if (c == ' ')
                tabs++;
            else
                break;
        }

        tabs /= 4;

        if (tabs < scope)
        {
            scope_increased = false;
            scope_decreased = true;
            scope--;
        }
        else if (tabs > scope)
        {
            scope_increased = true;
            scope_decreased = false;
        }
        else
        {
            scope_increased = false;
            scope_decreased = false;
        }

        if (scope_decreased)
            outfile << std::string(scope, '\t') + "\t}\n";

        std::cout << "scope: " << scope << ", tabs: " << tabs << std::endl;

        strutil::replace_all(line, "\n", "");
        strutil::replace_all(line, "\r", "");
        strutil::trim_left(line);

        // * operators
        for (auto &unary_operator : syntax::operators::unary)
            strutil::replace_all(line, unary_operator.first + " ", unary_operator.second);
        for (auto &binary_operator : syntax::operators::binary)
            strutil::replace_all(line, " " + binary_operator.first + " ", binary_operator.second);

        // * lib functions
        for (auto &stl_function : syntax::stl_functions)
        {
            strutil::replace_all(line, stl_function.first, stl_function.second);

            std::size_t location = line.find(stl_function.second);

            if (location != std::string::npos)
            {
                line.replace(line.find(stl_function.second) + stl_function.second.length() + 1, 1, ")");
            }
        }

        std::vector<std::string> tokens = strutil::split(line, ' ');
        std::cout << "new first token: \"" << tokens[0] << "\"" << std::endl;

        // ! DEBUG: print line
        std::cout << line_count << std::flush;
        std::cout << "tokens length: " << tokens.size() << std::endl;
        for (std::string token : tokens)
            std::cout << " " << token << std::flush;
        std::cout << std::endl;

        // * if empty line
        if (tokens.size() == 0)
            continue;

        // * set
        if (tokens[0] == "set")
        {
            std::cout << "\tset sentence encountered" << std::endl;
            // * if four tokens are given
            if (tokens.size() == 4)
            {
                std::string variable = tokens[1];
                std::string value = tokens[3];
                std::string new_line;

                // * if variable hasn't been declared
                if (variables.find(variable) == variables.end())
                {
                    util::var_data data(variable);
                    variables[variable] = data;

                    new_line = "\tauto " + variable + " = " + value + ";\n";
                }
                // * if it has
                else
                    new_line = std::string(scope, '\t') + "\t" + variable + " = " + value + ";\n";

                outfile << new_line;
            }
            // * if five tokens are given
            else if (tokens.size() == 5)
            {
                std::string type = tokens[1];
                std::string variable = tokens[2];
                std::string value = tokens[4];
                std::string new_line;

                // * if variable hasn't been declared
                if (variables.find(variable) == variables.end())
                {
                    util::var_data data = util::var_data(type, variable);
                    variables[variable] = data;

                    new_line = "\t" + syntax::types.at(type) + " " + variable + " = " + value + ";\n";
                }

                else
                {
                    new_line = "\t" + variable + " = " + value + ";\n";
                }

                outfile << new_line;
            }
        }
        // * declare
        else if (tokens[0] == "declare")
        {
            std::cout << "\tdeclare sentence encountered" << std::endl;

            std::string new_line;
            std::string type = tokens[1];
            std::string variable = tokens[2];
            std::cout << "variable: " << variable << std::endl;
            std::cout << "type: " << type << std::endl;

            util::var_data data = util::var_data(variable);
            variables[variable] = data;

            new_line = std::string(scope, '\t') + "\t" + syntax::types.at(type) + " " + variable + ";\n";
            outfile << new_line;
        }
        // * if
        else if (tokens[0] == "if")
        {
            std::cout << "\tif sentence encountered" << std::endl;

            std::string new_line;
            std::string condition = tokens[1];
            condition.pop_back();

            new_line = std::string(scope, '\t') + "\tif (" + condition + ") {\n";
            outfile << new_line;

            scope++;
            scope_increased = true;
        }
        // * for
        else if (tokens[0] == "for")
        {
            std::cout << "\tfor sentence encountered" << std::endl;

            std::string new_line;
            std::string variable = tokens[1];
            std::string min = tokens[4];
            std::string max = tokens[6];
            max.pop_back();

            new_line = std::string(scope, '\t') + "\tfor (int i = " + min + "; i <= " + max + "; i++) {\n";
            outfile << new_line;

            scope++;
            scope_increased = true;
        }
        // * while
        else if (tokens[0] == "while")
        {
            std::cout << "\tfor sentence encountered" << std::endl;

            std::string new_line;
            std::string condition = tokens[1];
            condition.pop_back();

            new_line = std::string(scope, '\t') + "\twhile (" + condition + ") {\n";
            outfile << new_line;

            scope++;
            scope_increased = true;
        }
        // * print
        else if (tokens[0] == "print")
        {
            std::string new_line;

            new_line = std::string(scope, '\t') + "\tstd::cout << " + tokens[1] + " << std::endl;\n";
            outfile << new_line;
        }
        // * add assignment
        else if (tokens[0] == "add")
        {
            std::string new_line;

            new_line = std::string(scope, '\t') + "\t" + tokens[3] + " += " + tokens[1] + ";\n";
            outfile << new_line;
        }
        // * subtract assignment
        else if (tokens[0] == "subtract")
        {
            std::string new_line;

            new_line = std::string(scope, '\t') + "\t" + tokens[3] + " -= " + tokens[1] + ";\n";
            outfile << new_line;
        }
        // * multiply assignment
        else if (tokens[0] == "multiply")
        {
            std::string new_line;

            new_line = std::string(scope, '\t') + "\t" + tokens[1] + " *= " + tokens[3] + ";\n";
            outfile << new_line;
        }
        // * divide assignment
        else if (tokens[0] == "divide")
        {
            std::string new_line;

            new_line = std::string(scope, '\t') + "\t" + tokens[1] + " /= " + tokens[3] + ";\n";
            outfile << new_line;
        }
        // * increment
        else if (tokens[0] == "increment")
        {
            std::string new_line;

            new_line = std::string(scope, '\t') + "\t" + tokens[1] + "++;\n";
            outfile << new_line;
        }
        // * decrement
        else if (tokens[0] == "decrement")
        {
            std::string new_line;

            new_line = std::string(scope, '\t') + "\t" + tokens[1] + "--;\n";
            outfile << new_line;
        }

        // * otherwise
        else
        {
            std::cout << "UNKNOWN TOKEN: \"" << tokens[0] << "\"" << std::endl;
        }
    }

    if (scope != 0)
        for (; scope > 0; scope--)
            outfile << std::string(scope - 1, '\t') + "\t}\n";

    outfile << "\treturn 0;\n"
               "}\n";
}