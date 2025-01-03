#ifndef PARSER_H
#define PARSER_H

#include <vector>
#include <string>
#include <memory>
#include <iostream>
#include <sstream>
#include <variant>

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

    Token(TokenType type, const std::string& lexeme, const std::string& literal, int line)
        : token_type(type), lexeme(lexeme), literal(literal), line(line)
    {
    }

    Token(std::string type, const std::string& lexeme, const std::string& literal, int line)
        : lexeme(lexeme), literal(literal), line(line)
    {
        getStringtoTokenType(type, token_type);
    }

    std::string toString() const
    {
        return lexeme + " " + literal;
    }

private:
    void getStringtoTokenType(const std::string& str, TokenType& type);
};

using EvalResult = std::variant<double, bool, std::string>;

class Expression
{
public:
    virtual ~Expression() = default;
    virtual std::string form_string()
    {
        return "";
    }
    virtual EvalResult evaluate() = 0;
    bool getstring = false;

protected:

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

    EvalResult evaluate() override
    {
        EvalResult left_result = left->evaluate();
        EvalResult right_result = right->evaluate();
        bool are_both_double = std::holds_alternative<double>(left_result) &&
            std::holds_alternative<double>(right_result);
        bool are_both_boolean = std::holds_alternative<bool>(left_result) &&
            std::holds_alternative<bool>(right_result);
        if (op.token_type == TokenType::GREATER)
        {
            if (are_both_double)
                return std::get<double>(left_result) > std::get<double>(right_result);
        }
        else if (op.token_type == TokenType::GREATER_EQUAL)
        {
            if (are_both_double)
                return std::get<double>(left_result) >= std::get<double>(right_result);
        }
        else if (op.token_type == TokenType::LESS)
        {
            if (are_both_double)
                return std::get<double>(left_result) < std::get<double>(right_result);
        }
        else if (op.token_type == TokenType::LESS_EQUAL)
        {
            if (are_both_double) return std::get<double>(left_result) <= std::get<double>(right_result);
        }
        else if (op.token_type == TokenType::EQUAL_EQUAL)
        {
            return are_both_double ? std::get<double>(left_result) == std::get<double>(right_result)
                : (!are_both_boolean ? (evaluateWithStringFlag(left) == evaluateWithStringFlag(right)) :
                    std::get<bool>(left_result) == std::get<bool>(right_result));
        }
        else if (op.token_type == TokenType::BANG_EQUAL)
        {
            return are_both_double ? std::get<double>(left_result) != std::get<double>(right_result)
                : (!are_both_boolean ? (evaluateWithStringFlag(left) != evaluateWithStringFlag(right)) :
                    std::get<bool>(left_result) != std::get<bool>(right_result));
        } 
        else if(op.lexeme == "+")
        {
            if (are_both_double)
                return std::get<double>(left_result) + std::get<double>(right_result);
            if(!are_both_boolean)
            {
                return std::get<std::string>(left_result) + std::get<std::string>(right_result);
            }
            throw std::runtime_error("Operands must be two numbers or two strings.");
        }
        else if(op.lexeme == "&&")
        {
            if (std::holds_alternative<bool>(left_result) && std::holds_alternative<bool>(right_result))
            {
                return std::get<bool>(left_result) && std::get<bool>(right_result);
            }
            throw std::runtime_error("Operands must be two booleans.");
        }
        else if(op.lexeme == "||")
        {
            if (std::holds_alternative<bool>(left_result) && std::holds_alternative<bool>(right_result))
            {
                return std::get<bool>(left_result) || std::get<bool>(right_result);
            }
            throw std::runtime_error("Operands must be two booleans.");
        }
        else if (are_both_double)
        {
            double left_val = std::get<double>(left_result);
            double right_val = std::get<double>(right_result);
            return (op.lexeme == "-" ? left_val - right_val : 
                       (op.lexeme == "*" ? left_val * right_val : 
                           (op.lexeme == "/" ? left_val / right_val : 0.0)));
        }
        throw std::runtime_error("Operands must be numbers.");
    }
    private:
        std::string evaluateWithStringFlag(std::shared_ptr<Expression> expr, bool flag = true) const {
            bool oldFlag = expr->getstring;
            expr->getstring = flag;
            EvalResult result = expr->evaluate();
            expr->getstring = oldFlag;
            return std::get<std::string>(result);
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

    EvalResult evaluate() override
    {
        EvalResult right_result = right->evaluate();

        if (std::holds_alternative<bool>(right_result))
        {
            if(op.lexeme == "!") return !std::get<bool>(right_result);
        }
        else if (std::holds_alternative<double>(right_result))
        {
            if (op.lexeme == "-")  return -std::get<double>(right_result);
            return false;
        }
        else if (std::holds_alternative<std::string>(right_result))
        {
            if (op.lexeme == "!")
                return (std::get<std::string>(right_result) == "nil");
        }

        throw std::runtime_error("Operand must be a number.");
    }
};

class Literal : public Expression
{
public:
    std::string value;

    Literal(const std::string& value) : value(value) {}

    virtual std::string form_string() override
    {
        return value;
    }

    EvalResult evaluate() override
    {
        if (value == "true")
        {
            return true;
        }
        else if (value == "false")
        {
            return false;
        }
        else
        {
            if(getstring)
            {
                return value;
            }
            try
            {
                return std::stod(value);
            }
            catch (const std::exception& e)
            {
                return value;
            }
        }
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

    EvalResult evaluate() override
    {
        return expression->evaluate();
    }
};

class Parser
{
public:
    Parser(const std::vector<Token>& tokens);
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

    bool  match(const std::vector<TokenType>& types);
    bool  check(TokenType type);
    bool  isAtEnd();
    Token advance();
    Token peek();
    Token previous();
    Token consume(TokenType type, const std::string& message);
    void  error(const Token& token, const std::string& message);
    void  report(int line, const std::string& where, const std::string& message);
};

#endif // PARSER_H
