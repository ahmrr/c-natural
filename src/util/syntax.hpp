#include <string>
#include <unordered_map>

#ifndef SYNTAX_HPP
#define SYNTAX_HPP

namespace syntax
{
    const std::unordered_map<std::string, std::string> types = {
        {"integer", "int"}};

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