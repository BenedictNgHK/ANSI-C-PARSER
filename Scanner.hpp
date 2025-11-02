#ifndef SCANNER_HPP
#define SCANNER_HPP
#include <iostream>
#include <fstream>
#include <sstream>
#include <list>
#include <cstdint>
#include <cstring>
#include <cstdbool>
#include <cstdlib>
#include <stack>
#include "Token.hpp"
#include "Error.hpp"
#include <queue>
#define EXTENSION ".c"

class Scanner
{
protected:
    std::list<Token> symbolTable;
    std::string pathToFile;
    int32_t lineNo; // Line No. of the source code file
    std::ifstream inputFile;
    std::list<Token>::iterator currentToken;

    struct Macro
    {
        std::vector<std::string> parameters;
        std::list<Token> tokens;
        Macro() : parameters(), tokens() {};
    };
    std::map<std::string, Macro> definedMacro;
    inline void addParameter(const std::string &para, Macro &m)
    {
        m.parameters.push_back(para);
    }
    void addToken(Macro &m);
    Error &loggedError;

    void handleStr(std::list<Token> &list);
    void handleChar(std::list<Token> &list);
    void handleDirective();
    void handleComment();

    inline bool isDefinedMacro(const std::string &macro)
    {
        return definedMacro.find(macro) != definedMacro.end();
    }
    inline std::map<std::string, Macro>::iterator getDefinedMacro(const std::string &macro)
    {
        return definedMacro.find(macro);
    }
    char handleEscape(char ch)
    {
        char ret = ch;
        if (isdigit(ch))
        {
            auto checkOct = [](char temp) -> bool
            {
                if (temp >= '0' && temp <= '7')
                    return true;
                else
                    return false;
            };
            char temp = ch;
            std::string oct;
            for (int i = 0; i < 3; i++)
            {
                if (checkOct(temp))
                {
                    oct.push_back(temp);
                    inputFile.get(temp);
                }
                else
                {
                    if (i == 0)
                    {
                        loggedError.addError(lineNo, ESCAPE_ERROR);
                        return temp;
                    }
                    break;
                }
            }
            inputFile.unget();
            int value;
            std::stringstream ss;
            ss << std::oct << oct;
            ss >> value;

            // Return the corresponding character
            return static_cast<char>(value);
        }

        switch (ch)
        {
        case ('\\'):
            ret = '\\';
            break;
        case ('\?'):
            ret = '\?';
            break;
        case ('\''):
            ret = '\'';
            break;
        case ('\"'):
            ret = '\"';
            break;
        case ('\v'):
            ret = '\v';
            break;

        case ('r'):
            ret = '\r';
            break;
        case ('n'):
            ret = '\n';
            break;
        case ('t'):
            ret = '\t';
            break;
        case ('a'):
            ret = '\a';
            break;
        case ('b'):
            ret = '\b';
            break;
        case ('x'):
        {
            char temp;
            std::string hex = "";
            for (int i = 0; i < 2; i++)
            {
                if (isxdigit(inputFile.peek()))
                {
                    temp = inputFile.get();
                    hex.push_back(temp);
                }
                else if (i == 0)
                {
                    loggedError.addError(lineNo, ESCAPE_ERROR);
                    return 'x';
                }
            }
            int value;
            std::stringstream ss;
            ss << std::hex << hex;
            ss >> value;

            // Return the corresponding character
            ret = static_cast<char>(value);

            break;
        }

        default:
            ret = ch;
        }
        return ret;
    }

    void appendList(std::list<Token> &list);
    void appendMacro(std::list<Token> &list);
    bool end;

public:
    Scanner(const std::string &path, Error &e);
    Scanner() = delete;
    Scanner(Scanner &s) = delete;
    Scanner(Scanner &&s) = delete;
    Scanner &operator=(Scanner s) = delete;
    Scanner &operator=(Scanner &s) = delete;
    Scanner &operator=(Scanner &&s) = delete;
    ~Scanner() { inputFile.close(); };

    inline std::list<Token>::iterator lastItr() { return symbolTable.end(); };

    inline int getlineNo() { return lineNo; }
    inline bool isEnd() { return inputFile.eof(); }
    std::list<Token>::iterator getNextToken();
    std::list<Token>::iterator peekNextToken();
    std::list<Token>::iterator peekPrevToken();
    std::list<Token>::iterator ungetToken();
    void printMacro(std::ostream &os)
    {
        for (auto macro : definedMacro)
        {
            os << macro.first << '\t' << std::endl;
            os << "Tokens:" << std::endl;

            for (auto token : macro.second.tokens)
            {
                TokenToString t;
                os << "Type:\t" << t(token.type) << "\tLexme:\t" << token.lexeme << std::endl;
            }
            os << "Parameters:" << std::endl;
            for (auto para : macro.second.parameters)
            {
                os << "parameter:\t" << para << std::endl;
            }
        }
    }
};

#endif