#include "parser.h"

bool getmiddlestring(std::string &tok, std::string &math_operator)
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
        return true;
    }
    size_t next_space = tok.find(" ", found + 1);
    math_operator     = tok.substr(found + 1, next_space - found - 1);
    return true;
}

void parse(const std::vector<std::string> &tokens, int &retVal)
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

    std::cout << ans << std::endl;
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