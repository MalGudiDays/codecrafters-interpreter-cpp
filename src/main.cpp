#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>

std::string read_file_contents(const std::string &filename);

void printTokens(const std::vector<std::string> &tokens)
{
    for(const std::string &token: tokens)
    {
        std::cout << token << std::endl;
    }
}

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

        if(command == "tokenize")
        {
            std::string file_contents = read_file_contents(args[1]);

            // Uncomment this block to pass the first stage

            if(!file_contents.empty())
            {
                std::istringstream          stream(file_contents);
                std::string                 line;
                std::vector<std::string>    tokens;
                std::map<char, std::string> token_map = {{'(', "LEFT_PAREN "},
                                                         {')', "RIGHT_PAREN "},
                                                         {'{', "LEFT_BRACE "},
                                                         {'}', "RIGHT_BRACE "},
                                                         {',', "COMMA "},
                                                         {'.', "DOT "},
                                                         {'-', "MINUS "},
                                                         {'+', "PLUS "},
                                                         {';', "SEMICOLON "},
                                                         {'*', "STAR "},
                                                         {'!', "BANG "},
                                                         {'=', "EQUAL "},
                                                         {'/', "SLASH "}};
                int                         line_num  = 1;
                while(std::getline(stream, line))
                {
                    for(int ii = 0; ii < line.size(); ii++)
                    {
                        char ch = line[ii];
                        if(token_map.find(ch) != token_map.end())
                        {
                            if(ch == '=' && ii && line[ii - 1] == '=')
                            {
                                tokens.pop_back();
                                tokens.push_back("EQUAL_EQUAL == null");
                            }
                            else if(ch == '=' && ii && line[ii - 1] == '!')
                            {
                                tokens.pop_back();
                                tokens.push_back("BANG_EQUAL != null");
                            }
                            else
                            {
                                tokens.push_back(token_map[ch] + ch + " null");
                            }
                        }
                        else
                        {
                            retVal = 65;
                            std::cerr << "[line " << line_num
                                      << "] Error: Unexpected character: " << ch
                                      << std::endl;
                        }
                    }
                    ++line_num;
                }
                printTokens(tokens);
            }
            std::cout << "EOF  null" << std::endl;
            // Placeholder, remove this line when
            // implementing the scanner
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
