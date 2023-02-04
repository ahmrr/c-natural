#include <string>
#include <unordered_map>

#ifndef SYNTAX_HPP
#define SYNTAX_HPP

namespace syntax
{
    namespace delimiters
    {
        const std::string comment = "#";
    };

    namespace operators
    {
        const std::unordered_map<std::string, std::string> unary;
        const std::unordered_map<std::string, std::string> binary = {
            {"plus", "+"},
            {"minus", "-"},
            {"times", "*"},
            {"divided by", "/"},
            {"modulo", "%"}};
    };
}

#endif