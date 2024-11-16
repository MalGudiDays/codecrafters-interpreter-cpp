#include "tokenize.h"
#include <iostream>
#include <sstream>
#include <algorithm>

const std::vector<std::string> reserved_words{"and",
                                              "class",
                                              "else",
                                              "false",
                                              "for",
                                              "fun",
                                              "if",
                                              "nil",
                                              "or",
                                              "print",
                                              "return",
                                              "super",
                                              "this",
                                              "true",
                                              "var",
                                              "while"};

Tokenizer::Tokenizer()
    : quote_index(-1), number_index(-1), literal_index(-1), line_num(1), retVal(0)
{
    token_map = {{'(', "LEFT_PAREN "},
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
}

void Tokenizer::tokenize(const std::string        &file_contents,
                         int                      &ret,
                         std::vector<std::string> &toks)
{
    if(!file_contents.empty())
    {
        std::istringstream stream(file_contents);
        std::string        line;
        while(std::getline(stream, line))
        {
            processLine(line);
            ++line_num;
        }
    }
    if(tokens.size())
    {
        tokens.push_back("EOF  null");
        toks = tokens;
    }
    // printTokens();
    ret = retVal;
}

void Tokenizer::processLine(const std::string &line)
{
    for(int i = 0; i < line.size(); ++i)
    {
        if(line[i] == '/' && i && line[i - 1] == '/')
        {
            tokens.pop_back();
            return;
        }
        processCharacter(line[i], i, line);
    }
}

void Tokenizer::processCharacter(char ch, int &index, const std::string &line)
{
    if(ch == '%' || ch == '#' || ch == '@' || ch == '$')
    {
        retVal = 65;
        std::cerr << "[line " << line_num << "] Error: Unexpected character: " << ch
                  << std::endl;
    }
    else if(ch == '"' && quote_index == -1)
    {
        handleQuote(ch, index, line);
    }
    else if((ch >= '0' && ch <= '9') || number_index != -1)
    {
        handleNumber(ch, index, line);
    }
    else if(ch == '\t' || ch == ' ')
    {
        return;
    }
    else if(token_map.find(ch) != token_map.end())
    {
        handleToken(ch, index, line);
    }
    else
    {
        handleLiteral(ch, index, line);
    }
}

void Tokenizer::handleQuote(char ch, int &index, const std::string &line)
{
    while(index < line.size())
    {
        ch = line[index];
        if(quote_index == -1)
        {
            quote_index = index;
        }
        else if(ch == '"')
        {
            tokens.push_back("STRING \"" + quote + "\" " + quote);
            quote_index = -1;
            quote.clear();
            return;
        }
        else
        {
            quote += ch;
        }
        ++index;
    }
    if(index == line.size() && quote_index != -1)
    {
        retVal = 65;
        std::cerr << "[line " << line_num << "] Error: Unterminated string." << std::endl;
    }
}

void Tokenizer::handleNumber(char ch, int &index, const std::string &line)
{
    while(index < line.size())
    {
        ch                = line[index];
        bool isDigitorDot = (ch >= '0' && ch <= '9') || ch == '.';
        if(number_index == -1)
        {
            number_index = index;
            number += ch;
        }
        else if(!isDigitorDot)
        {
            finalizeNumber();
            if(index < line.size() && index > 0)
            {
                --index;
            }
            return;
        }
        else
        {
            number += ch;
        }
        ++index;
    }
    if(index == line.size() && number_index != -1)
    {
        finalizeNumber();
    }
}

void Tokenizer::handleToken(char ch, int &index, const std::string &line)
{
    if(token_map.find(ch) != token_map.end())
    {
        if(ch == '=' && index && line[index - 1] == '=' && tokens.size() &&
           tokens.back() == "EQUAL = null")
        {
            tokens.pop_back();
            tokens.push_back("EQUAL_EQUAL == null");
        }
        else if(ch == '=' && index && line[index - 1] == '!' && tokens.size() &&
                tokens.back() == "BANG ! null")
        {
            tokens.pop_back();
            tokens.push_back("BANG_EQUAL != null");
        }
        else if(ch == '=' && index && line[index - 1] == '<' && tokens.size() &&
                tokens.back() == "LESS < null")
        {
            tokens.pop_back();
            tokens.push_back("LESS_EQUAL <= null");
        }
        else if(ch == '=' && index && line[index - 1] == '>' && tokens.size() &&
                tokens.back() == "GREATER > null")
        {
            tokens.pop_back();
            tokens.push_back("GREATER_EQUAL >= null");
        }
        else if(ch == '/' && index && line[index - 1] == '/' && tokens.size() &&
                tokens.back() == "SLASH / null")
        {
            tokens.pop_back();
            return;
        }
        else
        {
            tokens.push_back(token_map[ch] + ch + " null");
        }
    }
    else
    {
        std::cerr << "[line " << line_num << "] Error: Unexpected character: " << ch
                  << std::endl;
    }
}

bool Tokenizer::isreserved(const std::string &ltrl)
{
    for(const std::string &word: reserved_words)
    {
        if(ltrl == word)
        {
            return true;
        }
    }
    return false;
}

void Tokenizer::finalizeLiteral(std::string &literal)
{
    std::string identifier = "IDENTIFIER";
    if(isreserved(literal))
    {
        identifier = literal;
        std::transform(identifier.begin(), identifier.end(), identifier.begin(), ::toupper);
    }
    tokens.push_back(identifier + " " + literal + " null");
    literal_index = -1;
    literal.clear();
}

void Tokenizer::handleLiteral(char ch, int &index, const std::string &line)
{
    while(index < line.size())
    {
        ch = line[index];
        bool isAlphaNumeric =
            ch != ' ' && ch != '\t' && token_map.find(ch) == token_map.end();
        if(literal_index == -1)
        {
            literal_index = index;
            literal += ch;
        }
        else
        {
            if(!isAlphaNumeric)
            {
                finalizeLiteral(literal);
                if(index < line.size() && index > 0)
                {
                    --index;
                }
                return;
            }
            else
            {
                literal += ch;
            }
        }
        ++index;
    }
    if(index == line.size() && literal_index != -1)
    {
        finalizeLiteral(literal);
    }
}

void Tokenizer::finalizeNumber()
{
    std::string numberstr = number;
    getnumstr(number, numberstr);
    tokens.push_back("NUMBER " + number + " " + numberstr);
    number_index = -1;
    number.clear();
}

void Tokenizer::printTokens() const
{
    for(const std::string &token: tokens)
    {
        std::cout << token << std::endl;
    }
}

void Tokenizer::getnumstr(std::string &number, std::string &numberstr)
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