#include "../util/strutil.hpp"
#include "../util/util.hpp"
#include "../util/syntax.hpp"
#include "../util/functions.hpp"
#include "lex.hpp"
#include <queue>

void lex::parse(std::ifstream &infile, std::ofstream &outfile)
{

    std::queue<std::string> functionProgram; 
    std::queue<std::string> structProgram; 
    std::queue<std::string> mainProgram;
    std::queue<std::string> *theQueue = &mainProgram;
    
    // * the line counter for the file
    util::u64 line_count = 0;

    // * beginning file template
    outfile << "#include <iostream>\n\n";
    theQueue->push("int main() {\n");

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
    // * if we are in struct, name of struct
    std::string structName;

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
            theQueue->push(std::string(scope, '\t') + "\t}");

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

        // * if an empty line is encountered, we don't need to do anything else
        if (tokens.size() == 0)
            continue;

        // * set statement
        if (tokens[0] == "set")
        {
            // * get
            if (tokens[3] == "element") {
                std::string dst_name = tokens[1];
                std::string index = tokens[5];
                std::string name = tokens[7];

                std::string new_line = std::string(scope, '\t') + "\t" + dst_name + " = " + name + "[" + index + "];\n";
                theQueue->push(new_line);

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

                    theQueue->push(new_line);
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

                    theQueue->push(new_line);
                }
            }
        }
        // * define statement
        else if (tokens[0] == "define") 
        {
            if (tokens[1] == "array") 
            {
                std::string variableName = tokens[2];
                std::string type = tokens[4];
                std::string size = tokens[7];

                std::string new_line = std::string(scope, '\t') + "\t" + syntax::types.at(type) + " " + variableName + "[" +
                        size + "];\n";
                theQueue->push(new_line);
            } else if (tokens[1] == "2darray") {
                std::string variableName = tokens[2];
                std::string type = tokens[4];
                std::string size = tokens[7];
                
            }
        }
        // * declare statement
        else if (tokens[0] == "declare")
        {
            // * 
            if (tokens[2] == "function") 
                {
                std::string type = tokens[1];
                std::string name = tokens[3];
                std::string new_line;
                theQueue = &functionProgram;

                //Has params
                if (tokens.size() > 4)
                {
                    util::fun_data function = util::fun_data(true, name, type);
                    functions[name] = function;

                    new_line = type + " " + name;
                    theQueue->push(new_line);

                    theQueue->push(std::string("("));

                    for (int i = 5; i < tokens.size(); i = i + 2) {
                        theQueue->push(std::string(tokens[i] + " " + tokens[i+1]));
                        if (i+2 < tokens.size()) {
                            theQueue->push(", ");
                        }
                    }

                    theQueue->push(std::string(") {\n"));
                    // std::vector<std::string> param_list(tokens.begin() + 5, tokens.end());
                    // std::unordered_map<std::string, std::string> params;
                    // int listSize = param_list.size();

                    // for (int i = 0; i < listSize; i++)
                    // {
                    //     std::string param_type = param_list[i];
                    //     std::string param_name = param_list[i + 1];
                    //     param_name.pop_back();

                    //     params[param_name] = param_type;
                    // }

                    // util::fun_data function = util::fun_data(true, name, type, params);
                    // functions[name] = function;

                    // new_line = type + " " + name + "(";

                    // for (auto const& pair : params)
                    //     new_line += pair.first + " " + pair.second + ", ";

                    // new_line.pop_back();
                    // new_line.pop_back();

                    // new_line +=  ");\n";
                }

                //No params
                else
                {
                    util::fun_data function = util::fun_data(true, name, type);
                    functions[name] = function;

                    new_line = type + " " + name + "();\n";
                    theQueue->push(new_line);
                }
            }

            else if (tokens[1] == "structure")
            {
                tokens[2].pop_back();
                std::string name = tokens[2];
                structName = name;

                std::string new_line;
                new_line = "typedef struct " + name + "_H {\n";

                theQueue = &structProgram;
                theQueue->push(new_line);
                scope++;
                scope_increased = true; 
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
                theQueue->push(new_line);
            }
        }
        // * if statement
        else if (tokens[0] == "if")
        {
            std::string new_line;
            std::string condition = tokens[1];
            condition.pop_back();

            new_line = std::string(scope, '\t') + "\tif (" + condition + ") {\n";
            mainProgram.push(new_line);

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
            theQueue->push(new_line);

            scope++;
            scope_increased = true;
        }
        // * else statement
        else if (tokens[0] == "otherwise," && !switched) 
        {
            std::string new_line = std::string(scope, '\t') + "else {\n";
            theQueue->push(new_line);

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

            theQueue->push(new_line);

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

            theQueue->push(new_line);

            scope++;
            scope_increased = true;
            if (first_case) first_case = false;
        }
        // * default statement
        else if (tokens[0] == "otherwise," && switched) 
        {
            std::string new_line;
            new_line = std::string(scope, '\t') + "default: {\n";

            theQueue->push(new_line);
            scope++;
            scope_increased = true;
            switched = false;
        }
        // * break statement
        else if (tokens[0] == "break") 
        {
            std::string new_line;
            new_line = std::string(scope, '\t') + "break;\n";
            theQueue->push(new_line);
        }
        // * end struct statement
        else if (tokens[0] == "end") {
            std::string new_line;
            if (theQueue == &functionProgram) {
                new_line = "}\n";
            } else {
                new_line = structName + ";\n";
            }
            theQueue->push(new_line);
            theQueue = &mainProgram;
        }
        // * return
        else if (tokens[0] == "return") {
            std::string new_line = "return";

            if (theQueue == &functionProgram) {

                if (tokens.size() > 1) {
                    new_line += " ";
                    for (int i = 1; i < tokens.size(); ++i) {
                        new_line += tokens[i];
                        if (i+1 < tokens.size()) {
                            new_line += " ";
                        }
                    }
                }
            }

            new_line += ";\n}\n";
            theQueue->push(new_line);
        }
        // * for statement
        else if (tokens[0] == "for")
        {
            std::string new_line;
            std::string variable = tokens[1];
            std::string min = tokens[4];
            std::string max = tokens[6];
            max.pop_back();

            new_line = std::string(scope, '\t') + "\tfor (int i = " + min + "; i <= " + max + "; i++) {\n";
            theQueue->push(new_line);

            scope++;
            scope_increased = true;
        }
        // * while statement
        else if (tokens[0] == "while")
        {
            std::string new_line;
            std::string condition = tokens[1];
            condition.pop_back();

            new_line = std::string(scope, '\t') + "\twhile (" + condition + ") {\n";
            theQueue->push(new_line);

            scope++;
            scope_increased = true;
        }
        // * print statement
        else if (tokens[0] == "print")
        {
            std::string new_line;

            new_line = std::string(scope, '\t') + "\tstd::cout << " + tokens[1] + " << std::endl;\n";
            theQueue->push(new_line);
        }
        // * add assignment statement
        else if (tokens[0] == "add")
        {
            std::string new_line;

            new_line = std::string(scope, '\t') + "\t" + tokens[3] + " += " + tokens[1] + ";\n";
            theQueue->push(new_line);
        }
        // * subtract assignment statement
        else if (tokens[0] == "subtract")
        {
            std::string new_line;

            new_line = std::string(scope, '\t') + "\t" + tokens[3] + " -= " + tokens[1] + ";\n";
            theQueue->push(new_line);
        }
        // * multiply assignment statement
        else if (tokens[0] == "multiply")
        {
            std::string new_line;

            new_line = std::string(scope, '\t') + "\t" + tokens[1] + " *= " + tokens[3] + ";\n";
            theQueue->push(new_line);
        }
        // * divide assignment statement
        else if (tokens[0] == "divide")
        {
            std::string new_line;

            new_line = std::string(scope, '\t') + "\t" + tokens[1] + " /= " + tokens[3] + ";\n";
            theQueue->push(new_line);
        }
        // * increment statement
        else if (tokens[0] == "increment")
        {
            std::string new_line;

            new_line = std::string(scope, '\t') + "\t" + tokens[1] + "++;\n";
            theQueue->push(new_line);
        }
        // * decrement statement
        else if (tokens[0] == "decrement")
        {
            std::string new_line;

            new_line = std::string(scope, '\t') + "\t" + tokens[1] + "--;\n";
            theQueue->push(new_line);
        }
        // * otherwise
        else
        {
            std::cout << "UNKNOWN TOKEN: \"" << tokens[0] << "\"" << std::endl;
        }
    }

    while (!functionProgram.empty()) {
        std::string functionProgramLine = functionProgram.front();
        functionProgram.pop();
        outfile << functionProgramLine;
    }

    while (!structProgram.empty()) {
        std::string structProgramLine = structProgram.front();
        structProgram.pop();
        outfile << structProgramLine;
    }

    // * "remove" all remaining levels of scope; add closing parentheses for each level of scope that still exists
    if (scope != 0)
        for (; scope > 0; scope--)
            mainProgram.push(std::string(scope - 1, '\t') + "\t}\n");

    while (!mainProgram.empty()) {
        std::string mainProgramLine = mainProgram.front();
        mainProgram.pop();
        outfile << mainProgramLine;
    }
            
    outfile << "\treturn 0;\n"
               "}\n";
}