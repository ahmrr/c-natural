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
    // * the line counter for the file
    util::u64 line_count = 0;

    // * beginning file template
    outfile << "#include <iostream>\n\n"
               "int main() {\n";

    // * hashmaps for variables and functions
    std::unordered_map<std::string, util::var_data> variables;
    std::unordered_map<std::string, util::fun_data> functions;

    // * level of scope
    util::u64 scope = 0;

    // * has the scope level increased since the last loop iteration?
    bool scope_increased = false;
    // * has the scope level decreased since the last loop iteration?
    bool scope_decreased = false;
    // * are we in a switch statement?
    bool switched = false;
    // * is the current case the first in a switch statement?
    bool first_case = true;

    while (infile.peek() != EOF)
    {
        // * get a line
        std::string line;
        std::getline(infile, line);
        line_count++;

        // * get actual current scope (# of tabs)
        util::u64 tabs = 0;

        for (char c : line)
        {
            if (c == ' ')
                tabs++;
            else
                break;
        }

        tabs /= TAB_SPACE;

        // * if the current scope is less than the expected scope
        if (tabs < scope)
        {
            scope_increased = false;
            scope_decreased = true;
            scope--;
        }
        // * if the current scope is more than the expected scope
        else if (tabs > scope)
        {
            scope_increased = true;
            scope_decreased = false;
        }
        // * if the current scope is the same as the expected scope
        else
        {
            scope_increased = false;
            scope_decreased = false;
        }

        // * output a closing bracket if the scope has decreased
        if (scope_decreased)
            outfile << std::string(scope, '\t') + "\t}\n";

        // ! DEBUG: print scope and tabs
        std::cout << "scope: " << scope << ", tabs: " << tabs << std::endl;

        // * trim the line; remove all leading whitespace, newlines, and return carriages
        strutil::replace_all(line, "\n", "");
        strutil::replace_all(line, "\r", "");
        strutil::trim_left(line);

        // * replace all C-Natural operators with valid C++ operators and operations
        for (auto &unary_operator : syntax::operators::unary)
            strutil::replace_all(line, unary_operator.first + " ", unary_operator.second);
        for (auto &binary_operator : syntax::operators::binary)
            strutil::replace_all(line, " " + binary_operator.first + " ", binary_operator.second);

        // * split the line into tokens
        std::vector<std::string> tokens = strutil::split(line, ' ');

        // ! DEBUG: print line
        std::cout << line_count << std::flush;
        std::cout << "tokens length: " << tokens.size() << std::endl;
        for (std::string token : tokens)
            std::cout << " " << token << std::flush;
        std::cout << std::endl;

        // * if an empty line is encountered, we don't need to do anything else
        if (tokens.size() == 0)
            continue;

        // * set statement
        if (tokens[0] == "set")
        {
            std::cout << "\tset sentence encountered" << std::endl;

            // * get
            if (tokens[3] == "element") {

                std::cout << "\telement sentence encountered of type ";

                std::string dst_name = tokens[1];
                std::string index = tokens[5];
                std::string name = tokens[7];

                std::string new_line = std::string(scope, '\t') + "\t" + dst_name + " = " + name + "[" + index + "];\n";
                outfile << new_line;

            } else {

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
                        new_line = std::string(scope, '\t') + "\t" + variable + " = " + value + ";\n";

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
        }
        // * define statement
        else if (tokens[0] == "define") {

            std::cout << "\tdefine sentence encountered of type ";

            if (tokens[1] == "array") {
                std::cout << "array" << std::endl;

                std::string variableName = tokens[2];
                std::string type = tokens[4];
                std::string size = tokens[7];

                std::string new_line = std::string(scope, '\t') + "\t" + syntax::types.at(type) + " " + variableName + "[" +
                        size + "];\n";
                outfile << new_line;
            }

        }
        // * declare statement
        else if (tokens[0] == "declare")
        {
            std::cout << "\tdeclare sentence encountered" << std::endl;

            // * 
            if (tokens[2] == "function") {
                std::string type = tokens[1];
                std::string name = tokens[3];
                std::string new_line;

                //Has params
                if (tokens.size() > 4)
                {
                    std::vector<std::string> param_list(tokens.begin() + 5, tokens.end());
                    std::unordered_map<std::string, std::string> params;

                    for (int i = 0; i < param_list.size(); i++)
                    {
                        std::string param_type = param_list[i];
                        std::string param_name = param_list[i + 1];
                        param_name.pop_back();

                        params[param_name] = param_type;
                    }

                    util::fun_data function = util::fun_data(true, name, type, params);
                    functions[name] = function;

                    new_line = type + " " + name + "(";

                    for (auto const& pair : params)
                        new_line += pair.first + " " + pair.second + ", ";

                    new_line.pop_back();
                    new_line.pop_back();

                    new_line +=  ");\n";
                }

                //No params
                else
                {
                    util::fun_data function = util::fun_data(true, name, type);
                    functions[name] = function;

                    new_line = type + " " + name + "();\n";
                }

                outfile << new_line;
            }

            else if (tokens[1] == "structure")
            {

            }

            else {
                std::string new_line;
                std::string type = tokens[1];
                std::string variable = tokens[2];
                std::cout << "variable: " << variable << std::endl;
                std::cout << "type: " << type << std::endl;

                util::var_data data = util::var_data(variable);
                variables[variable] = data;

                new_line = std::string(scope, '\t') + "\t" + syntax::types.at(type) + " " + variable + ";\n";
                outfile << new_line;
            }
        }
        // * if statement
        else if (tokens[0] == "if")
        {
            std::cout << "\tif sentence encountered" << std::endl;

            std::string new_line;
            std::string condition = tokens[1];
            condition.pop_back();

            new_line = std::string(scope, '\t') + "\tif (" + condition + ") {\n";
            outfile << new_line;

            scope++;
            scope_increased = true;
        }
        // * else if  statement
        else if (tokens[0] == "otherwise" && tokens[1] == "if") 
        {   
            std::string new_line;
            std::string condition = tokens[2];
            condition.pop_back();

            new_line = std::string(scope, '\t') + "\telse if (" + condition + ") {\n";
            outfile << new_line;

            scope++;
            scope_increased = true;
        }
        // * else statement
        else if (tokens[0] == "otherwise," && !switched) 
        {
            std::string new_line = std::string(scope, '\t') + "else {\n";
            outfile << new_line;

            scope++;
            scope_increased = true;
        }
        // * switch statement
        else if (tokens[0] == "when")
        {
            first_case = true;

            std::string new_line;
            tokens[1].pop_back();
            
            new_line = std::string(scope, '\t') + "switch(" + tokens[1] + ") {\n";

            outfile << new_line;

            scope++;
            scope_increased = true;
            switched = true;
        }
        // * case statement
        else if (tokens[0] == "is")
        {
            std::string new_line;
            tokens[1].pop_back();

            new_line = std::string(scope, '\t') + "case " + tokens[1] + ": {\n";

            outfile << new_line;

            scope++;
            scope_increased = true;
            if (first_case) first_case = false;
        }
        // * default statement
        else if (tokens[0] == "otherwise," && switched) 
        {
            std::string new_line;
            new_line = std::string(scope, '\t') + "default: {\n";

            outfile << new_line;
            scope++;
            scope_increased = true;
            switched = false;
        }
        // * break statement
        else if (tokens[0] == "break") {
            std::string new_line;
            new_line = std::string(scope, '\t') + "break;\n";
            outfile << new_line;
        }
        // * for statement
        else if (tokens[0] == "for")
        {
            std::cout << "\tfor sentence encountered" << std::endl;

            std::string new_line;
            std::string variable = tokens[1];
            std::string min = tokens[4];
            std::string max = tokens[6];
            max.pop_back();

            new_line = std::string(scope, '\t') + "\tfor (int i = " + min + "; i <= " + max + "; i++) {\n";
            outfile << new_line;

            scope++;
            scope_increased = true;
        }
        // * while statement
        else if (tokens[0] == "while")
        {
            std::cout << "\tfor sentence encountered" << std::endl;

            std::string new_line;
            std::string condition = tokens[1];
            condition.pop_back();

            new_line = std::string(scope, '\t') + "\twhile (" + condition + ") {\n";
            outfile << new_line;

            scope++;
            scope_increased = true;
        }
        // * print statement
        else if (tokens[0] == "print")
        {
            std::string new_line;

            new_line = std::string(scope, '\t') + "\tstd::cout << " + tokens[1] + " << std::endl;\n";
            outfile << new_line;
        }
        // * add assignment statement
        else if (tokens[0] == "add")
        {
            std::string new_line;

            new_line = std::string(scope, '\t') + "\t" + tokens[3] + " += " + tokens[1] + ";\n";
            outfile << new_line;
        }
        // * subtract assignment statement
        else if (tokens[0] == "subtract")
        {
            std::string new_line;

            new_line = std::string(scope, '\t') + "\t" + tokens[3] + " -= " + tokens[1] + ";\n";
            outfile << new_line;
        }
        // * multiply assignment statement
        else if (tokens[0] == "multiply")
        {
            std::string new_line;

            new_line = std::string(scope, '\t') + "\t" + tokens[1] + " *= " + tokens[3] + ";\n";
            outfile << new_line;
        }
        // * divide assignment statement
        else if (tokens[0] == "divide")
        {
            std::string new_line;

            new_line = std::string(scope, '\t') + "\t" + tokens[1] + " /= " + tokens[3] + ";\n";
            outfile << new_line;
        }
        // * increment statement
        else if (tokens[0] == "increment")
        {
            std::string new_line;

            new_line = std::string(scope, '\t') + "\t" + tokens[1] + "++;\n";
            outfile << new_line;
        }
        // * decrement statement
        else if (tokens[0] == "decrement")
        {
            std::string new_line;

            new_line = std::string(scope, '\t') + "\t" + tokens[1] + "--;\n";
            outfile << new_line;
        }
        // * otherwise
        else
        {
            std::cout << "UNKNOWN TOKEN: \"" << tokens[0] << "\"" << std::endl;
        }
    }

    // * "remove" all remaining levels of scope; add closing parentheses for each level of scope that still exists
    if (scope != 0)
        for (; scope > 0; scope--)
            outfile << std::string(scope - 1, '\t') + "\t}\n";

    outfile << "\treturn 0;\n"
               "}\n";
}