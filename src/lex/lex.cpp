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

    while (infile.peek() != EOF)
    {
        std::string line;
        std::getline(infile, line);
        line_count++;

        strutil::replace_all(line, "\n", "");
        strutil::replace_all(line, "\r", "");

        // * operators
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
            // * if four tokens are given
            if (tokens.size() == 4)
            {
                std::string variable = tokens[1];
                std::string value = tokens[3];

                std::string new_line = "\tauto " + variable + " = " + value + ";\n";
                outfile << new_line;
            }
        }
    }

    outfile << "}\n";
}