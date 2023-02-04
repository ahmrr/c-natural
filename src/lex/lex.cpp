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

    outfile << "#include <iostream>\n\n"
               "int main() {\n";

    std::unordered_map<std::string, util::var_data> variables;

    while (infile.peek() != EOF)
    {
        std::string line;
        std::getline(infile, line);
        line_count++;

        strutil::replace_all(line, "\n", "");
        strutil::replace_all(line, "\r", "");

        // * operators
        for (auto &unary_operator : syntax::operators::unary)
            strutil::replace_all(line, unary_operator.first + " ", unary_operator.second);
        for (auto &binary_operator : syntax::operators::binary)
            strutil::replace_all(line, " " + binary_operator.first + " ", binary_operator.second);

        std::vector<std::string> tokens = strutil::split(line, ' ');

        // ! DEBUG: print line
        std::cout << line_count << std::flush;
        for (std::string token : tokens)
            std::cout << " " << token << std::flush;
        std::cout << std::endl;

        // * if empty line
        if (tokens.size() == 0)
            continue;

        // * if a set sentence is encountered
        if (tokens[0] == "set")
        {
            std::cout << "set sentence encountered" << std::endl;
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
                    new_line = "\t" + variable + " = " + value + ";\n";

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
        // * if declare sentence
        else if (tokens[0] == "declare")
        {
            std::cout << "declare sentence encountered" << std::endl;

            std::string new_line;
            std::string type = tokens[1];
            std::string variable = tokens[2];
            std::cout << "variable: " << variable << std::endl;
            std::cout << "type: " << type << std::endl;

            util::var_data data = util::var_data(variable);
            variables[variable] = data;

            new_line = "\t" + syntax::types.at(type) + " " + variable + ";\n";
            outfile << new_line;
        }
    }

    outfile << "\treturn 0;\n"
               "}\n";
}