#ifndef ERROR_HPP
#define ERROR_HPP
#include <iostream>
#include <map>
#include <string>
#include <vector>
#define ESCAPE_ERROR "Incorrect Escape Character"
#define STRING_ERROR "Incorrect String Syntax"
#define CHAR_ERROR "Incorrect Char Syntax"
#define COMMENT_ERROR "Comment Error: Missing close symbol"
#define INCLUD_ERROR "Include Syntax is wrong"
#define DEFINE_ERROR "Define Syntax is wrong"
#define STRUCT_UNION_ERROR "Struct/Union define Error"
#define MAIN_ERROR "Main function Error"
class Error
{
public:
    std::map<int, std::string> errors;

public:
    Error() = default;
    void addError(int line, const std::string &error);
    void printError(std::ostream &os);

    ~Error() = default;
};
#endif