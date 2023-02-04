#include <fstream>

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
}

#endif