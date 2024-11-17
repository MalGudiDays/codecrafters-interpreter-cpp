#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
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
                // tokens.pop_back();
                if(retVal == 0)
                {
                    std::vector<Token> tokenList;
                    for(const std::string &token: tokens)
                    {
                        std::istringstream iss(token);
                        std::string        tktype;
                        iss >> tktype;
                        if(tktype == "STRING")
                        {
                            int first = token.find_first_of('"');
                            int last  = token.find_last_of('"');
                            if(first != std::string::npos && last != std::string::npos &&
                               first < last)
                            {
                                std::string lexeme =
                                    token.substr(first + 1, last - first - 1);
                                tokenList.push_back(
                                    Token(TokenType::STRING, lexeme, lexeme, 0));
                                continue;
                            }
                            continue;
                        }
                        std::string lexeme;
                        iss >> lexeme;
                        std::string literal;
                        if(!(iss >> literal))
                        {
                            literal = " ";
                        }
                        tokenList.push_back(Token(tktype, lexeme, literal, 0));
                    }
                    Parser                      parser(tokenList);
                    std::shared_ptr<Expression> expr = parser.parse();
                    if(expr == nullptr)
                    {
                        return 65;
                    }
                    std::cout << expr->form_string() << std::endl;
                }
            }
            else
            {
                for(const std::string &token: tokens)
                {
                    std::cout << token << std::endl;
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
