#include <string>
#include <unordered_map>

#ifndef SYNTAX_HPP
#define SYNTAX_HPP

namespace syntax
{
    const std::unordered_map<std::string, std::string> types = {
        {"integer", "int"},
        {"float", "float"},
        {"character", "char"},
        {"double", "double"}
    };

    const std::unordered_map<std::string, std::string> stl_functions = {
        {"byte size of ", "sizeof("},
        {"sqrt of ", "sqrt("}};

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
            {"modulo", "%"},
            {"is equal to", "=="},
            {"is not equal to", "!="},
            {"is greater than", ">"},
            {"is less than", "<"},
            {"is less than or equal to", "<="},
            {"is greater than or equal to", ">="}};
    };
}

#endif