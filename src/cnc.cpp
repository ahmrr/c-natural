#include "lex/lex.hpp"

#include <iostream>
#include <fstream>

int main(int argc, char **argv)
{
    if (argc < 3)
    {
        std::cout << "usage: " << argv[0] << " <infile> <outfile>" << std::endl;
        return 1;
    }

    std::ifstream infile(argv[1]);
    std::ofstream outfile(argv[2]);

    lex::parse(infile, outfile);

    return 0;
}