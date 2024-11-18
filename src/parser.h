#ifndef PARSER_H
#define PARSER_H

#include <vector>
#include <string>
#include <memory>
#include <iostream>

enum class TokenType
{
    LEFT_PAREN,
    RIGHT_PAREN,
    LEFT_BRACE,
    RIGHT_BRACE,
    COMMA,
    DOT,
    MINUS,
    PLUS,
    SEMICOLON,
    SLASH,
    STAR,
    BANG,
    BANG_EQUAL,
    EQUAL,
    EQUAL_EQUAL,
    GREATER,
    GREATER_EQUAL,
    LESS,
    LESS_EQUAL,
    IDENTIFIER,
    STRING,
    NUMBER,
    AND,
    CLASS,
    ELSE,
    FALSE,
    FUN,
    FOR,
    IF,
    NIL,
    OR,
    PRINT,
    RETURN,
    SUPER,
    THIS,
    TRUE,
    VAR,
    WHILE,
    END_OF_FILE
};

class Token
{
  public:
    TokenType   token_type;
    std::string lexeme;
    std::string literal;
    int         line;

    Token(TokenType type, const std::string &lexeme, const std::string &literal, int line)
        : token_type(type), lexeme(lexeme), literal(literal), line(line)
    {
    }

    Token(std::string type, const std::string &lexeme, const std::string &literal, int line)
        : lexeme(lexeme), literal(literal), line(line)
    {
        getStringtoTokenType(type, token_type);
    }

    std::string toString() const
    {
        return lexeme + " " + literal;
    }

  private:
    void getStringtoTokenType(const std::string &str, TokenType &type);
};

class Expression
{
  public:
    virtual ~Expression() = default;
    virtual std::string form_string()
    {
        return prefix_string;
    }
    virtual double evaluate() = 0;

  protected:
    std::string prefix_string = "";

  private:
};

class Binary : public Expression
{
  public:
    std::shared_ptr<Expression> left;
    Token                       op;
    std::shared_ptr<Expression> right;

    Binary(std::shared_ptr<Expression> left, Token op, std::shared_ptr<Expression> right)
        : left(left), op(op), right(right)
    {
    }
    virtual std::string form_string() override
    {
        return "(" + op.lexeme + " " + left->form_string() + " " + right->form_string() +
               ")";
    }

    double evaluate() override
    {
        double leftValue  = left->evaluate();
        double rightValue = right->evaluate();

        switch(op.token_type)
        {
        case TokenType::PLUS:
            return leftValue + rightValue;
        case TokenType::MINUS:
            return leftValue - rightValue;
        case TokenType::STAR:
            return leftValue * rightValue;
        case TokenType::SLASH:
            return leftValue / rightValue;
        default:
            throw std::runtime_error("Unknown binary operator");
        }
    }
};

class Unary : public Expression
{
  public:
    Token                       op;
    std::shared_ptr<Expression> right;

    Unary(Token op, std::shared_ptr<Expression> right) : op(op), right(right) {}
    virtual std::string form_string() override
    {
        return "(" + op.lexeme + " " + right->form_string() + ")";
    }

    double evaluate() override
    {
        double rightValue = right->evaluate();

        switch(op.token_type)
        {
        case TokenType::MINUS:
            return -rightValue;
        case TokenType::BANG:
            return !rightValue;
        default:
            throw std::runtime_error("Unknown unary operator");
        }
    }
};

class Literal : public Expression
{
  public:
    std::string value;

    Literal(const std::string &value) : value(value) {}

    virtual std::string form_string() override
    {
        return value;
    }

    double evaluate() override
    {
        std::cout << value << std::endl;
        std::throw_with_nested(std::runtime_error("Literal evaluation not implemented"));
        return 0.0;
    }
};

class Grouping : public Expression
{
  public:
    std::shared_ptr<Expression> expression;

    Grouping(std::shared_ptr<Expression> expression) : expression(expression) {}
    virtual std::string form_string() override
    {
        return "(group " + expression->form_string() + ")";
    }

    double evaluate() override
    {
        return expression->evaluate();
    }
};

class Parser
{
  public:
    Parser(const std::vector<Token> &tokens);
    std::shared_ptr<Expression> parse();

  private:
    std::vector<Token> tokens;
    int                current;

    std::shared_ptr<Expression> expression();
    std::shared_ptr<Expression> equality();
    std::shared_ptr<Expression> comparison();
    std::shared_ptr<Expression> term();
    std::shared_ptr<Expression> factor();
    std::shared_ptr<Expression> unary();
    std::shared_ptr<Expression> primary();

    bool  match(const std::vector<TokenType> &types);
    bool  check(TokenType type);
    bool  isAtEnd();
    Token advance();
    Token peek();
    Token previous();
    Token consume(TokenType type, const std::string &message);
    void  error(const Token &token, const std::string &message);
    void  report(int line, const std::string &where, const std::string &message);
};

#endif // PARSER_H
