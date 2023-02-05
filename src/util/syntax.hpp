#ifndef SYNTAX_HPP
#define SYNTAX_HPP
#include <string>
#include <unordered_map>

namespace syntax
{

    const std::unordered_map<std::string, std::string> types = {
        {"integer", "int"},
        {"character", "char"},
        {"boolean", "bool"},
        {"nothing", "NULL"}};

    namespace delimiters
    {
        const std::string comment = "#";
        const std::string string = "\"";
        const std::string array_left = "[";
        const std::string array_right = "]";
    };

    namespace operators
    {
        const std::unordered_map<std::string, std::string> unary = {
            {"location of", "&"},
            {"value at", "*"},
            {"byte size of ", "sizeof "}};

        const std::unordered_map<std::string, std::string> binary = {
            {"plus", "+"},
            {"minus", "-"},
            {"times", "*"},
            {"divided by", "/"},
            {"modulo", "%"},
            {"is equal to", "=="},
            {"is not equal to", "!="},
            {"is greater than", ">"},
            {"is less than", "<"},
            {"is less than or equal to", "<="},
            {"is greater than or equal to", ">="},
            {"and", "&&"},
            {"or", "||"},
            {"not", "!"},
            {"AND", "&"},
            {"OR", "|"},
            {"XOR", "^"},
            {"NOT", "~"},
            {"shifted left by", "<<"},
            {"shifted right by", ">>"}};
    };
}

#endif