#include "parser.h"

bool getmiddlestring(std::string &tok, std::string &math_operator)
{
    size_t found = tok.find(" ");
    if(tok.substr(0, found) == "EOF")
    {
        return false;
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
    for(const auto &node: nodes)
    {
        std::string math_operator;
        std::string tok = node.get()->val;
        if(!getmiddlestring(tok, math_operator))
        {
            retVal = 64;
            break;
        }
        if(math_operator == "+" || math_operator == "-" || math_operator == "*" ||
           math_operator == "/")
        {
            if(stack.size() < 2)
            {
                std::cerr << "Invalid expression" << std::endl;
                retVal = 64;
                return;
            }

            Node right = stack.back();
            stack.pop_back();

            Node left = stack.back();
            stack.pop_back();

            node->left  = left;
            node->right = right;
        }

        stack.push_back(node);
    }

    if(stack.size() != 1)
    {
        std::cerr << "Invalid expression" << std::endl;
        retVal = 1;
        return;
    }

    std::cout << stack[0]->val << std::endl;
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