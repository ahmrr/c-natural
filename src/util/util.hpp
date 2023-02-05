#include <fstream>
#include <vector>
#include <string>
#include <unordered_map>

#ifndef UTIL_HPP
#define UTIL_HPP

namespace util
{
    typedef uint_fast8_t u8;
    typedef uint_fast16_t u16;
    typedef uint_fast32_t u32;
    typedef uint_fast64_t u64;

    class var_data
    {
    public:
        var_data()
        {
            this->declared = false;
            this->type = "NULL";
            this->name = "NULL";
        }

        var_data(std::string name)
        {
            this->declared = true;
            this->type = "auto";
            this->name = name;
        }

        var_data(std::string type, std::string name)
        {
            this->declared = true;
            this->type = type;
            this->name = name;
        }

        bool declared;
        std::string type;
        std::string name;
    };

    class fun_data
    {
    public:
        // Default constructor
        fun_data()
        {
            declared = false;
            name = "NULL";
            type = "NULL";
            params = {};
        }

        // No params
        fun_data(bool declared, std::string name, std::string type)
        {
            this->declared = declared;
            this->name = name;
            this->type = type;
            this->params = {};
        }

        // Regular function
        fun_data(bool declared, std::string name, std::string type, std::unordered_map<std::string, std::string> params)
        {
            this->declared = declared;
            this->name = name;
            this->type = type;
            this->params = params;
        }

        bool declared;
        std::string name;
        std::string type;
        std::unordered_map<std::string, std::string> params;
    };

    std::vector<std::string> tokenize(std::string line);
}

#endif