#include "parser.h"
#include <iostream>
#include <stdexcept>
#include <unordered_map>

Parser::Parser(const std::vector<Token> &tokens) : tokens(tokens), current(0) {}

std::shared_ptr<Expression> Parser::parse()
{
    try
    {
        return expression();
    }
    catch(const std::runtime_error &e)
    {
        return nullptr;
    }
}

std::shared_ptr<Expression> Parser::expression()
{
    return equality();
}

std::shared_ptr<Expression> Parser::equality()
{
    std::shared_ptr<Expression> expr = comparison();

    while(match({TokenType::BANG_EQUAL, TokenType::EQUAL_EQUAL}))
    {
        Token                       op    = previous();
        std::shared_ptr<Expression> right = comparison();
        expr                              = std::make_shared<Binary>(expr, op, right);
    }

    return expr;
}

std::shared_ptr<Expression> Parser::comparison()
{
    std::shared_ptr<Expression> expr = term();

    while(match({TokenType::GREATER,
                 TokenType::GREATER_EQUAL,
                 TokenType::LESS,
                 TokenType::LESS_EQUAL}))
    {
        Token                       op    = previous();
        std::shared_ptr<Expression> right = term();
        expr                              = std::make_shared<Binary>(expr, op, right);
    }

    return expr;
}

std::shared_ptr<Expression> Parser::term()
{
    std::shared_ptr<Expression> expr = factor();

    while(match({TokenType::MINUS, TokenType::PLUS}))
    {
        Token                       op    = previous();
        std::shared_ptr<Expression> right = factor();
        expr                              = std::make_shared<Binary>(expr, op, right);
    }

    return expr;
}

std::shared_ptr<Expression> Parser::factor()
{
    std::shared_ptr<Expression> expr = unary();

    while(match({TokenType::SLASH, TokenType::STAR}))
    {
        Token                       op    = previous();
        std::shared_ptr<Expression> right = unary();
        expr                              = std::make_shared<Binary>(expr, op, right);
    }

    return expr;
}

std::shared_ptr<Expression> Parser::unary()
{
    if(match({TokenType::BANG, TokenType::MINUS}))
    {
        Token                       op    = previous();
        std::shared_ptr<Expression> right = unary();
        return std::make_shared<Unary>(op, right);
    }

    return primary();
}

std::shared_ptr<Expression> Parser::primary()
{
    if(match({TokenType::FALSE})) return std::make_shared<Literal>("false");
    if(match({TokenType::TRUE})) return std::make_shared<Literal>("true");
    if(match({TokenType::NIL})) return std::make_shared<Literal>("nil");

    if(match({TokenType::NUMBER, TokenType::STRING}))
    {
        return std::make_shared<Literal>(previous().literal);
    }

    if(match({TokenType::LEFT_PAREN}))
    {
        std::shared_ptr<Expression> expr = expression();
        consume(TokenType::RIGHT_PAREN, "Expect ')' after expression.");
        return std::make_shared<Grouping>(expr);
    }

    throw std::runtime_error("Expect expression.");
}

bool Parser::match(const std::vector<TokenType> &types)
{
    for(TokenType type: types)
    {
        if(check(type))
        {
            advance();
            return true;
        }
    }
    return false;
}

bool Parser::check(TokenType type)
{
    if(isAtEnd()) return false;
    return peek().token_type == type;
}

bool Parser::isAtEnd()
{
    return peek().token_type == TokenType::END_OF_FILE;
}

Token Parser::advance()
{
    if(!isAtEnd()) current++;
    return previous();
}

Token Parser::peek()
{
    return tokens[current];
}

Token Parser::previous()
{
    return tokens[current - 1];
}

Token Parser::consume(TokenType type, const std::string &message)
{
    if(check(type)) return advance();
    error(peek(), message);
    throw std::runtime_error(message);
}

void Parser::error(const Token &token, const std::string &message)
{
    if(token.token_type == TokenType::END_OF_FILE)
    {
        report(token.line, " at end", message);
    }
    else
    {
        report(token.line, " at '" + token.lexeme + "'", message);
    }
}

void Parser::report(int line, const std::string &where, const std::string &message)
{
    // std::cerr << "[line " << line << "] Error" << where << ": " << message << std::endl;
}

void Token::getStringtoTokenType(const std::string &str, TokenType &type)
{
    static const std::unordered_map<std::string, TokenType> tokenMap = {
        {"LEFT_PAREN", TokenType::LEFT_PAREN},
        {"RIGHT_PAREN", TokenType::RIGHT_PAREN},
        {"LEFT_BRACE", TokenType::LEFT_BRACE},
        {"RIGHT_BRACE", TokenType::RIGHT_BRACE},
        {"COMMA", TokenType::COMMA},
        {"DOT", TokenType::DOT},
        {"MINUS", TokenType::MINUS},
        {"PLUS", TokenType::PLUS},
        {"SEMICOLON", TokenType::SEMICOLON},
        {"SLASH", TokenType::SLASH},
        {"STAR", TokenType::STAR},
        {"BANG", TokenType::BANG},
        {"BANG_EQUAL", TokenType::BANG_EQUAL},
        {"EQUAL", TokenType::EQUAL},
        {"EQUAL_EQUAL", TokenType::EQUAL_EQUAL},
        {"GREATER", TokenType::GREATER},
        {"GREATER_EQUAL", TokenType::GREATER_EQUAL},
        {"LESS", TokenType::LESS},
        {"LESS_EQUAL", TokenType::LESS_EQUAL},
        {"IDENTIFIER", TokenType::IDENTIFIER},
        {"STRING", TokenType::STRING},
        {"NUMBER", TokenType::NUMBER},
        {"AND", TokenType::AND},
        {"CLASS", TokenType::CLASS},
        {"ELSE", TokenType::ELSE},
        {"FALSE", TokenType::FALSE},
        {"FUN", TokenType::FUN},
        {"FOR", TokenType::FOR},
        {"IF", TokenType::IF},
        {"NIL", TokenType::NIL},
        {"OR", TokenType::OR},
        {"PRINT", TokenType::PRINT},
        {"RETURN", TokenType::RETURN},
        {"SUPER", TokenType::SUPER},
        {"THIS", TokenType::THIS},
        {"TRUE", TokenType::TRUE},
        {"VAR", TokenType::VAR},
        {"WHILE", TokenType::WHILE},
        {"EOF", TokenType::END_OF_FILE}};
    auto it = tokenMap.find(str);
    if(it != tokenMap.end())
    {
        type = it->second;
    }
    else
    {
        throw std::runtime_error("Unknown token type: " + str);
    }
}
