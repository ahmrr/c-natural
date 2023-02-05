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
    // * the entire program, sent to the output file at the end of the parse
    std::string program;
    // * the "functions" section of the program, sent to the output file at the end of the parse
    std::string function_header;

    // * the line counter for the file
    util::u64 line_count = 0;

    // * beginning file template
    program += "int main() {\n";

    // * hashmaps for variables in main, variables in functions, and functions
    std::unordered_map<std::string, util::var_data> main_variables;
    std::unordered_map<std::string, util::var_data> function_variables;
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
    // * are we in a function (do closing parentheses need to be added to the top of the file)?
    bool function_scope = false;
    // * the string that is going to contain the conversion from C-Natural to C++
    std::string new_line;

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

        // * output a closing bracket if the scope has decreased; where to output depends on function scope
        if (scope_decreased && function_scope)
        {
            std::cout << "🚧 decreased scope while in function scope; program is " << program << std::endl;
            function_header += std::string(scope, '\t') + "}\n\n";
            function_scope = false;
        }
        else if (scope_decreased)
            program += std::string(scope, '\t') + "\t}\n";

        // * remove comments from the line
        line = strutil::split(line, syntax::delimiters::comment)[0];

        // ! DEBUG: print scope and tabs
        std::cout << "|- scope: " << scope << ", tabs: " << tabs << std::endl;

        // * indent: this is the calculated correct indentation for this line
        std::string indent = std::string(scope + !function_scope, '\t');

        // * trim the line; remove all leading and trailing whitespace, newlines, and return carriages
        strutil::replace_all(line, "\n", "");
        strutil::replace_all(line, "\r", "");
        strutil::trim_left(line);
        strutil::trim_right(line);

        // * replace all C-Natural operators with valid C++ unary and binary operators and operations
        for (auto &unary_operator : syntax::operators::unary)
            strutil::replace_all(line, unary_operator.first + " ", unary_operator.second);
        for (auto &binary_operator : syntax::operators::binary)
            strutil::replace_all(line, " " + binary_operator.first + " ", binary_operator.second);

        // * split the line into tokens
        std::vector<std::string> tokens = util::tokenize(line);

        // ! DEBUG: print line
        std::cout << "|- tokens length: " << tokens.size() << std::endl;
        std::cout << "🔵 " << line_count << std::flush;
        for (std::string token : tokens)
            std::cout << " " << token << std::flush;
        std::cout << std::endl;

        // * if an empty line is encountered, we don't need to do anything else
        if (tokens.size() == 0)
            continue;

        // * set statement
        if (tokens[0] == "set")
        {
            std::cout << "|- set sentence encountered" << std::endl;

            // * get
            if (tokens[3] == "element")
            {

                std::cout << "|- element sentence encountered of type ";

                std::string dst_name = tokens[1];
                std::string index = tokens[5];
                std::string name = tokens[7];

                std::string new_line = indent + dst_name + " = " + name + "[" + index + "];\n";
            }
            else
            {
                // * if four tokens are given
                if (tokens.size() == 4)
                {
                    std::string variable = tokens[1];
                    std::string value = tokens[3];

                    // * if variable hasn't been declared in main or function scope
                    if (main_variables.find(variable) == main_variables.end() || function_variables.find(variable) == function_variables.end())
                    {
                        util::var_data data(variable);
                        if (function_scope)
                            function_variables[variable] = data;
                        else
                            main_variables[variable] = data;

                        new_line = indent + "auto " + variable + " = " + value + ";\n";
                    }
                    // * if it has been declared already in main or function scope
                    else
                        new_line = indent + variable + " = " + value + ";\n";
                }
                // * if five tokens are given
                else if (tokens.size() == 5)
                {
                    std::string type = syntax::types.at(tokens[1]);
                    std::string variable = tokens[2];
                    std::string value = tokens[4];

                    // * if variable hasn't been declared
                    if (main_variables.find(variable) == main_variables.end() || function_variables.find(variable) == function_variables.end())
                    {
                        util::var_data data(variable);
                        if (function_scope)
                            function_variables[variable] = data;
                        else
                            main_variables[variable] = data;

                        std::cout << "🚧 type: " << type << std::endl;

                        new_line = indent + type + " " + variable + " = " + value + ";\n";
                    }
                    else
                        new_line = indent + variable + " = " + value + ";\n";
                }
            }
        }
        // * define statement
        else if (tokens[0] == "define")
        {
            std::cout << "|- define sentence encountered of type ";

            // * if we are defining an array
            if (tokens[1] == "array")
            {
                std::cout << "array" << std::endl;

                std::string variableName = tokens[2];
                std::string type = syntax::types.at(tokens[4]);
                std::string size = tokens[7];

                new_line = indent + type + " " + variableName + "[" +
                           size + "];\n";
            }
        }
        // * declare statement
        else if (tokens[0] == "declare")
        {
            std::cout << "|- declare sentence encountered" << std::endl;

            // * if a function is being declared
            if (tokens[2] == "function")
            {
                std::string type = syntax::types.at(tokens[1]);
                std::string name = tokens[3];

                // * if the function has parameters
                if (tokens.size() > 4)
                {
                    std::vector<std::string> param_list(tokens.begin() + 5, tokens.end());
                    std::unordered_map<std::string, std::string> params;

                    for (util::u32 i = 0; i < param_list.size(); i += 2)
                    {
                        std::string param_type = syntax::types.at(param_list[i]);
                        std::string param_name = param_list[i + 1];
                        if (i != param_list.size() - 2)
                            param_name.pop_back();

                        params[param_name] = param_type;
                        std::cout << "🚧 "
                                  << "iteration #" << i << " complete; param_type was " << param_type << ", param_name was " << param_name << std::endl;
                    }

                    util::fun_data function = util::fun_data(true, name, type, params);
                    functions[name] = function;

                    new_line = type + " " + name + "(";

                    for (auto const &pair : params)
                        new_line += pair.second + " " + pair.first + ", ";

                    new_line.pop_back();
                    new_line.pop_back();

                    new_line += ") {\n";

                    scope++;
                    scope_increased = true;
                    function_scope = true;
                }
                // * if the function doesn't have parameters
                else
                {
                    util::fun_data function = util::fun_data(true, name, type);
                    functions[name] = function;

                    new_line = type + " " + name + "() {\n";

                    scope++;
                    scope_increased = true;
                    function_scope = true;
                }
            }
            else if (tokens[1] == "structure")
            {
            }
            else
            {
                std::string type = tokens[1];
                std::string variable = tokens[2];

                util::var_data data = util::var_data(variable);
                // variables[variable] = data;

                new_line = indent + syntax::types.at(type) + " " + variable + ";\n";
            }
        }
        // * if statement
        else if (tokens[0] == "if")
        {
            std::cout << "|- if sentence encountered" << std::endl;

            std::string condition = tokens[1];
            condition.pop_back();

            new_line = indent + "if (" + condition + ") {\n";

            scope++;
            scope_increased = true;
        }
        // * else if  statement
        else if (tokens[0] == "otherwise" && tokens[1] == "if")
        {
            std::string condition = tokens[2];
            condition.pop_back();

            new_line = indent + "else if (" + condition + ") {\n";

            scope++;
            scope_increased = true;
        }
        // * else statement
        else if (tokens[0] == "otherwise," && !switched)
        {
            new_line = indent + "else {\n";

            scope++;
            scope_increased = true;
        }
        // * switch statement
        else if (tokens[0] == "when")
        {
            first_case = true;

            std::string condition = tokens[1];
            condition.pop_back();

            new_line = indent + "switch(" + condition + ") {\n";

            scope++;
            scope_increased = true;
            switched = true;
        }
        // * case statement
        else if (tokens[0] == "is")
        {
            std::string condition = tokens[1];
            condition.pop_back();

            new_line = indent + "case " + condition + ": {\n";

            scope++;
            scope_increased = true;
            if (first_case)
                first_case = false;
        }
        // * default statement
        else if (tokens[0] == "otherwise," && switched)
        {
            new_line = indent + "default: {\n";

            scope++;
            scope_increased = true;
            switched = false;
        }
        // * break statement
        else if (tokens[0] == "break")
        {
            new_line = indent + "break;\n";
        }
        // * for statement
        else if (tokens[0] == "for")
        {
            std::cout << "|- for sentence encountered" << std::endl;

            std::string variable = tokens[1];
            std::string min = tokens[4];
            std::string max = tokens[6];
            max.pop_back();

            new_line = indent + "for (int i = " + min + "; i <= " + max + "; i++) {\n";

            scope++;
            scope_increased = true;
        }
        // * while statement
        else if (tokens[0] == "while")
        {
            std::cout << "|- while sentence encountered" << std::endl;

            std::string condition = tokens[1];
            condition.pop_back();

            new_line = indent + "while (" + condition + ") {\n";

            scope++;
            scope_increased = true;
        }
        // * print statement
        else if (tokens[0] == "print")
            new_line = indent + "std::cout << " + tokens[1] + " << std::endl;\n";
        // * add assignment statement
        else if (tokens[0] == "add")
            new_line = indent + tokens[3] + " += " + tokens[1] + ";\n";
        // * subtract assignment statement
        else if (tokens[0] == "subtract")
            new_line = indent + tokens[3] + " -= " + tokens[1] + ";\n";
        // * multiply assignment statement
        else if (tokens[0] == "multiply")
            new_line = indent + tokens[1] + " *= " + tokens[3] + ";\n";
        // * divide assignment statement
        else if (tokens[0] == "divide")
            new_line = indent + tokens[1] + " /= " + tokens[3] + ";\n";
        // * increment statement
        else if (tokens[0] == "increment")
            new_line = indent + tokens[1] + "++;\n";
        // * decrement statement
        else if (tokens[0] == "decrement")
            new_line = indent + tokens[1] + "--;\n";
        // * otherwise
        else
        {
            std::cout << "🔴 unknown token: \"" << tokens[0] << "\"" << std::endl;
            continue;
        }

        if (function_scope)
            function_header += new_line;
        else
            program += new_line;
    }

    // * "remove" all remaining levels of scope; add closing parentheses for each level of scope that still exists
    for (; scope > 0; scope--)
    {
        if (function_scope)
        {
            function_header += std::string(scope - 1, '\t') + "}\n\n";
            function_scope = false;
        }
        else
            program += std::string(scope - 1, '\t') + "\t}\n";
    }

    // * finish program and write to file
    program += "\treturn 0;\n"
               "}\n";
    program = "#include <iostream>\n\n" + function_header + program;

    outfile << program;
}