// preprocessor directives
#ifndef PARSER_H
#define PARSER_H

#include <iostream>
#include <string>
#include <vector>
#include <memory>

class TreeNode
{
  public:
    std::string               val;
    std::shared_ptr<TreeNode> left;
    std::shared_ptr<TreeNode> right;

    TreeNode(const std::string &val) : val(val), left(nullptr), right(nullptr) {}
};

using Node = std::shared_ptr<TreeNode>;

class Parser
{
  public:
    static void parse(const std::vector<std::string> &tokens, int &retVal);

  private:
    static bool getmiddlestring(const std::string &tok, std::string &math_operator);
};

#endif // PARSER_H