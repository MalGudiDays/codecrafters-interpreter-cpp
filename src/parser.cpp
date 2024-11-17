#include "parser.h"
#include <stack>
#include <sstream>

int precedence(char op)
{
    if(op == '+' || op == '-') return 1;
    if(op == '*' || op == '/') return 2;
    return 0;
}

std::string reverseString(const std::string &str)
{
    std::string reversed = str;
    std::reverse(reversed.begin(), reversed.end());
    return reversed;
}

std::string Parser::infixToPrefix(const std::string &infix)
{
    std::string      reversedInfix = reverseString(infix);
    std::stack<char> s;
    std::string      postfix = "";

    for(char ch: reversedInfix)
    {
        if(std::isspace(ch))
        {
            continue;
        }
        else if(std::isdigit(ch) || std::isalpha(ch) || ch == '.')
        {
            postfix += ch;
        }
        else if(ch == ')')
        {
            s.push(ch);
        }
        else if(ch == '(')
        {
            while(!s.empty() && s.top() != ')')
            {
                postfix += (" " + std::string(1, s.top()));
                s.pop();
            }
            if(!s.empty() && s.top() == ')')
            {
                s.pop();
            }
        }
        else
        {
            postfix += " ";
            while(!s.empty() && precedence(s.top()) > precedence(ch))
            {
                postfix += (std::string(1, s.top()) + " ");
                s.pop();
            }
            s.push(ch);
        }
    }

    while(!s.empty())
    {
        postfix += s.top();
        s.pop();
    }

    // modify prefix string to add appropriate parenthesis if not present
    std::string             prefix = reverseString(postfix), _prefix = "", ans = "";
    std::istringstream      iss(prefix);
    std::string             tk;
    std::stack<std::string> st;
    std::string             prev_string = "";
    auto                    isCharSp    = [](char c) -> bool {
        return c == '+' || c == '-' || c == '*' || c == '/';
    };
    while(iss >> tk)
    {
        if(isCharSp(tk[0]))
        {
            s.push(tk[0]);
        }
        else
        {
            st.push(tk);
            if(st.size() == 2)
            {
                ans += "(" + std::string(1, s.top()) + " ";
                st.pop();
                s.pop();
                ans += st.top() + " " + tk + ")";
                st.pop();
                st.push(ans);
                ans = "";
            }
        }
    }
    if(!st.empty())
    {
        ans = st.top();
        st.pop();
    }
    return ans;
}

bool Parser::getmiddlestring(const std::string &tok, std::string &math_operator)
{
    size_t      found       = tok.find(" ");
    std::string currliteral = tok.substr(0, found);
    if(currliteral == "EOF")
    {
        return false;
    }
    else if(currliteral == "NUMBER")
    {
        size_t next_space = tok.find(" ", found + 1);
        math_operator     = tok.substr(next_space + 1);
        return true;
    }
    else if(currliteral == "STRING")
    {
        found             = tok.find('\"');
        size_t next_space = tok.find('\"', found + 1);
        math_operator     = "group " + tok.substr(found + 1, next_space - found - 1);
        return true;
    }
    else if(currliteral == "LEFT_PAREN" || currliteral == "RIGHT_PAREN" ||
            currliteral == "LEFT_BRACE" || currliteral == "RIGHT_BRACE")
    {
        size_t next_space = tok.find(' ', found + 1);
        math_operator     = tok.substr(found + 1, next_space - found - 1);
        if(currliteral == "LEFT_PAREN" || currliteral == "LEFT_BRACE")
        {
            // math_operator = math_operator + "group ";
        }
        return true;
    }
    else if(currliteral == "MINUS" || currliteral == "BANG")
    {
        size_t next_space = tok.find(' ', found + 1);
        math_operator     = tok.substr(found + 1, next_space - found - 1);
        math_operator     = "(" + math_operator + " ";
        opened_brace++;
        return true;
    }
    size_t next_space = tok.find(" ", found + 1);
    math_operator     = tok.substr(found + 1, next_space - found - 1);
    if(opened_brace)
    {
        math_operator = math_operator + ")";
        opened_brace--;
    }
    return true;
}

void Parser::parse(const std::vector<std::string> &tokens, int &retVal)
{
    if(tokens.empty())
    {
        std::cerr << "No tokens to parse" << std::endl;
        retVal = 1;
        return;
    }

    std::vector<Node> nodes;
    for(const auto &token: tokens)
    {
        nodes.push_back(std::make_shared<TreeNode>(token));
    }

    if(nodes.size() == 1)
    {
        std::string math_operator = "";
        std::string tok           = nodes[0].get()->val;
        bool        val           = getmiddlestring(tok, math_operator);
        if(!val)
        {
            retVal = 64;
            std::cerr << "Invalid expression" << std::endl;
            return;
        }
        std::cout << math_operator << std::endl;
        return;
    }

    std::vector<Node> stack;
    std::string       ans = "";
    for(const auto &node: nodes)
    {
        std::string math_operator;
        std::string tok = node.get()->val;
        if(!getmiddlestring(tok, math_operator))
        {
            retVal = 64;
            break;
        }
        ans += math_operator;
    }
    while(opened_brace > 0)
    {
        ans += ")";
        opened_brace--;
    }
    if(ans.size() && (ans[0] != '(' && ans[0] != '{'))
    {
        ans = "(" + ans + ")";
    }
    auto ans2 = infixToPrefix(ans);
    std::cout << ans2 << std::endl;
}

void PostOrderTraversal(TreeNode *root)
{
    if(root == nullptr)
    {
        return;
    }

    PostOrderTraversal(root->left.get());
    PostOrderTraversal(root->right.get());
    std::cout << root->val << " ";
}