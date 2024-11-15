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

void getnumstr(std::string &number, std::string &numberstr)
{
    size_t foundat = number.find('.');
    if(foundat == std::string::npos)
    {
        numberstr = number + ".0";
        return;
    }
    int i = number.size() - 1;
    while(i > foundat + 1 && number[i] == '0')
    {
        i--;
    }
    numberstr = number.substr(0, i + 1);
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
                                                         {'<', "LESS "},
                                                         {'>', "GREATER "},
                                                         {'/', "SLASH "}};
                int                         line_num  = 1;
                while(std::getline(stream, line))
                {
                    int         quote_index  = -1;
                    std::string quote        = "";
                    int         number_index = -1;
                    std::string number       = "";
                    for(int ii = 0; ii < line.size(); ii++)
                    {
                        char ch = line[ii];
                        if(ch == '"' || quote_index != -1)
                        {
                            if(quote_index == -1)
                            {
                                quote_index = ii;
                            }
                            else if(ch == '"' && line[ii - 1] != '\\')
                            {
                                tokens.push_back("STRING \"" + quote + "\" " + quote);
                                quote_index = -1;
                                quote       = "";
                            }
                            else
                            {
                                quote += ch;
                            }
                            continue;
                        }
                        bool isDigitorDot = (ch >= '0' && ch <= '9') || ch == '.';
                        if((ch >= '0' && ch <= '9') || number_index != -1)
                        {
                            if(number_index == -1)
                            {
                                number_index = ii;
                                number += ch;
                            }
                            else if(!isDigitorDot)
                            {
                                std::string numberstr = number;
                                getnumstr(number, numberstr);
                                tokens.push_back("NUMBER " + number + " " + numberstr);
                                number_index = -1;
                                number       = "";
                                if(ii < line.size() && ii > 0)
                                {
                                    ii--;
                                }
                            }
                            else
                                number += ch;
                            continue;
                        }
                        if(token_map.find(ch) != token_map.end())
                        {
                            if(ch == '=' && ii && line[ii - 1] == '=' && tokens.size() &&
                               tokens.back() == "EQUAL = null")
                            {
                                tokens.pop_back();
                                tokens.push_back("EQUAL_EQUAL == null");
                            }
                            else if(ch == '=' && ii && line[ii - 1] == '!' &&
                                    tokens.size() && tokens.back() == "BANG ! null")
                            {
                                tokens.pop_back();
                                tokens.push_back("BANG_EQUAL != null");
                            }
                            else if(ch == '=' && ii && line[ii - 1] == '<' &&
                                    tokens.size() && tokens.back() == "LESS < null")
                            {
                                tokens.pop_back();
                                tokens.push_back("LESS_EQUAL <= null");
                            }
                            else if(ch == '=' && ii && line[ii - 1] == '>' &&
                                    tokens.size() && tokens.back() == "GREATER > null")
                            {
                                tokens.pop_back();
                                tokens.push_back("GREATER_EQUAL >= null");
                            }
                            else if(ch == '/' && ii && line[ii - 1] == '/' &&
                                    tokens.size() && tokens.back() == "SLASH / null")
                            {
                                tokens.pop_back();
                                break;
                            }
                            else
                            {
                                tokens.push_back(token_map[ch] + ch + " null");
                            }
                        }
                        else if(ch == '\t' || ch == ' ')
                        {
                            continue;
                        }
                        else
                        {
                            retVal = 65;
                            std::cerr << "[line " << line_num
                                      << "] Error: Unexpected character: " << ch
                                      << std::endl;
                        }
                    }
                    if(quote_index != -1)
                    {
                        retVal = 65;
                        std::cerr << "[line " << line_num
                                  << "] Error: Unterminated string." << std::endl;
                    }
                    if(number_index != -1)
                    {
                        std::string numberstr = number;
                        getnumstr(number, numberstr);
                        tokens.push_back("NUMBER " + number + " " + numberstr);
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
