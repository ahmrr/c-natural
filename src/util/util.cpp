#include <vector>
#include <string>
#include <iostream>

#include "util.hpp"
#include "syntax.hpp"

std::vector<std::string> util::tokenize(std::string line)
{
    std::vector<std::string> tokens = {""};
    bool in_string = false;
    util::u32 array_level = 0;

    std::cout << "🚧 line is \"" << line << "\"" << std::endl;

    for (char c : line)
    {
        std::cout << "🚧 c is |" << c << "|" << std::endl;
        // * if a string literal delimiter is found, then either a string has ended or a new one has started
        if (c == syntax::delimiters::string[0])
        {
            std::cout << "🚧 found string" << std::endl;
            in_string = !in_string;
        }
        // * if an opening array delimiter is found, then the nesting level increases
        else if (c == syntax::delimiters::array_left[0])
            array_level++;
        // * if a closing array delimiter is found, then the nesting level decreases
        else if (c == syntax::delimiters::array_right[0])
            array_level--;
        // * if a space is found outside of any literals, tokenize it
        else if (c == ' ' && !in_string && array_level == 0)
        {
            std::cout << "🚧 found space" << std::endl;
            tokens.push_back("");
            continue;
        }

        tokens[tokens.size() - 1] += c;
    }

    return tokens;
}