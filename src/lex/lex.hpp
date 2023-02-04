#include <fstream>

#ifndef LEX_HPP
#define LEX_HPP

namespace lex
{
    /**
     * @brief parses the input file
     *
     * @param infile file to parse
     *
     * Parses file line-by-line and splits it into tokens
     */
    void parse(std::ifstream &infile, std::ofstream &outfile);
}

#endif