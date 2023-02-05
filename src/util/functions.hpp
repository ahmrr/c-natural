#ifndef FUNCTIONS_HPP
#define FUNCTIONS_HPP
#include <string>

namespace variables {

    std::string getSetString(std::string type, std::string nameVar, std::string value) {
        std::string returnString = type + " " + nameVar + " = " + value + ";";
        return returnString; 
    }

    std::string getSetString(std::string nameVar, std::string value) {
        std::string returnString = "auto " + nameVar + " = " + value + ";";
        return returnString;
    }

    std::string getDeclaredSetString(std::string src, std::string dst) {
        std::string returnString = dst + " = " + src + ";";
        return returnString;
    }

    std::string getDeclaredString(std::string type, std::string nameVar) {
        std::string returnString = type + " " + nameVar;
        return returnString;
    }

    std::string getTypecastString(std::string typeTo, std::string var) {
        std::string returnString = "(" + typeTo + ") " + var;
        return returnString;
    }

    std::string getConstantString(std::string type, std::string var, std::string value) {
        std::string returnString = "const " + type + " " + var + " = " + value + ";";
        return returnString;
    }

}

#endif