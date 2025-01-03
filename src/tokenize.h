#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <string>
#include <vector>
#include <map>

class Tokenizer
{
  public:
    Tokenizer();
    void tokenize(const std::string        &file_contents,
                  int                      &ret,
                  std::vector<std::string> &toks);

  private:
    void processLine(const std::string &line);
    void processCharacter(char ch, int &index, const std::string &line);
    void handleLiteral(char ch, int &index, const std::string &line);
    void handleQuote(char ch, int &index, const std::string &line);
    void handleNumber(char ch, int &index, const std::string &line);
    void handleToken(char ch, int &index, const std::string &line);
    void finalizeNumber();
    bool isreserved(const std::string &literal);
    void finalizeLiteral(std::string &literal);
    void printTokens() const;
    void getnumstr(std::string &number, std::string &numberstr);
    void handlenewline(char ch, int& index, const std::string& line);

    std::vector<std::string>    tokens;
    std::map<char, std::string> token_map;
    int                         quote_index;
    std::string                 quote;
    int                         number_index;
    std::string                 number;
    int                         literal_index;
    std::string                 literal;
    int                         line_num;
    int                         retVal;
};

#endif // TOKENIZER_H
