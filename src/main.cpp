#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include "tokenize.h"
#include "parser.h"

std::string read_file_contents(const std::string& filename)
{
    std::ifstream file(filename);
    if (!file.is_open())
    {
        std::cerr << "Error reading file: " << filename << std::endl;
        std::exit(1);
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    file.close();

    return buffer.str();
}

// Helper function to extract a string token
void extractStringToken(const std::string& token, std::vector<Token>& tokenList) {
    int first = token.find_first_of('"');
    int last = token.find_last_of('"');
    if (first != std::string::npos && last != std::string::npos && first < last) {
        std::string lexeme = token.substr(first + 1, last - first - 1);
        tokenList.push_back(Token(TokenType::STRING, lexeme, lexeme, 0));
    }
}

// Helper function to extract a general token
void extractGeneralToken(const std::string& token, std::vector<Token>& tokenList) {
    std::istringstream iss(token);
    std::string tktype;
    iss >> tktype;
    std::string lexeme;
    iss >> lexeme;
    std::string literal;
    if (!(iss >> literal)) {
        literal = " ";
    }
    tokenList.push_back(Token(tktype, lexeme, literal, 0));
}

void tokenConvert(const std::vector<std::string> &tokens, std::vector<Token> &tokenList)
{
    for (const std::string& token : tokens) {
        std::istringstream iss(token);
        std::string tktype;
        iss >> tktype;
        if (tktype == "STRING") {
            extractStringToken(token, tokenList);
        }
        else {
            extractGeneralToken(token, tokenList);
        }
    }
}

// Function to evaluate the parsed expression
void evaluateExpression(const std::shared_ptr<Expression>& expr) {
    try {
        auto d = expr->evaluate();
        if (std::holds_alternative<double>(d))
            std::cout << std::get<double>(d) << std::endl;
        else if (std::holds_alternative<bool>(d)) {
            if (std::get<bool>(d))
                std::cout << "true" << std::endl;
            else
                std::cout << "false" << std::endl;
        }
        else
            std::cout << std::get<std::string>(d) << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << '\n';
        exit(70);
    }
}

// Function to parse the expression
std::shared_ptr<Expression> parseExpression(Parser& parser) {
    std::shared_ptr<Expression> expr = parser.parse();
    if (expr == nullptr) {
        throw std::runtime_error("Failed to parse expression");
    }
    return expr;
}

// Function to evaluate or print the parsed expression
void evaluateOrPrintExpression(const std::shared_ptr<Expression>& expr, const std::string& command) {
    if (command == "evaluate" || command == "run") {
        evaluateExpression(expr);
    }
    else {
        std::cout << expr->form_string() << std::endl;
    }
}

// Function to print tokens
void printTokens(const std::vector<std::string>& tokens) {
    for (const std::string& token : tokens) {
        std::cout << token << std::endl;
    }
}

// Function to process the command
void processCommand(const std::string& command, const std::string& filename) {
    std::string file_contents = read_file_contents(filename);

    std::vector<std::string> tokens;
    Tokenizer tokenizer;
    int retVal = 0;
    tokenizer.tokenize(file_contents, retVal, tokens);

    if (command == "parse" || command == "evaluate" || command == "run") {
        if (retVal) exit(retVal);

        std::vector<Token> tokenList;
        tokenConvert(tokens, tokenList);
        if (tokenList.size() && tokenList[0].token_type == TokenType::PRINT) {
            tokenList.erase(tokenList.begin());
        }
        Parser parser(tokenList);
        try {
            std::shared_ptr<Expression> expr = parseExpression(parser);
            evaluateOrPrintExpression(expr, command);
        }
        catch (const std::exception& e) {
            std::cerr << e.what() << std::endl;
            exit(65);
        }
    }
    else if(command == "tokenize")
    {
        printTokens(tokens);
    }
    else {
    }
}

int main(int argc, char *argv[])
{
    int retVal = 0;
    // Disable output buffering
    std::cout << std::unitbuf;
    std::cerr << std::unitbuf;

    try {
        std::vector<std::string> args(argv + 1, argv + argc);
        if (args.size() < 2) {
            std::cerr << "Usage: ./your_program tokenize <filename>" << std::endl;
            return 64;
        }

        const std::string command = args[0];
        const std::string filename = args[1];

        if (command == "tokenize" || command == "parse" || command == "evaluate" || command == "run") {
            processCommand(command, filename);
        }
        else {
            std::cerr << "Unknown command: " << command << std::endl;
            return 1;
        }
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << '\n';
        return 1;
    }

    return retVal;
}
