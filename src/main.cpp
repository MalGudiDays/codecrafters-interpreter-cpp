#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include "tokenize.h"
#include "parser.h"

std::string read_file_contents(const std::string &filename);

int main(int argc, char *argv[])
{
    int retVal = 0;
    // Disable output buffering
    std::cout << std::unitbuf;
    std::cerr << std::unitbuf;

    try
    {
        std::vector<std::string> args(argv + 1, argv + argc);
        if(args.size() < 2)
        {
            std::cerr << "Usage: ./your_program tokenize <filename>" << std::endl;
            return 64;
        }

        const std::string command = args[0];

        if(command == "tokenize" || command == "parse")
        {
            std::string file_contents = read_file_contents(args[1]);

            std::vector<std::string> tokens;
            Tokenizer                tokenizer;
            tokenizer.tokenize(file_contents, retVal, tokens);
            if(command == "parse")
            {
                tokens.pop_back();
                if(retVal == 0)
                {
                    parse(tokens, retVal);
                }
            }
        }
        else
        {
            std::cerr << "Unknown command: " << command << std::endl;
            return 1;
        }
    }
    catch(const std::exception &e)
    {
        std::cerr << e.what() << '\n';
    }

    return retVal;
}

std::string read_file_contents(const std::string &filename)
{
    std::ifstream file(filename);
    if(!file.is_open())
    {
        std::cerr << "Error reading file: " << filename << std::endl;
        std::exit(1);
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    file.close();

    return buffer.str();
}
