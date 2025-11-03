#include "Scanner.hpp"
#define MODIFIED
Scanner::Scanner(const std::string &path, Error &e) : lineNo(1), loggedError(e)
{

    if (path.substr(path.size() - strlen(EXTENSION), strlen(EXTENSION)) != EXTENSION)
    {
        std::cerr << "The extension of file should be " << EXTENSION << std::endl;
        exit(1);
    }
    end = false;
    std::string buffer;
    inputFile.open(path, std::ifstream::in);
    TokenToString tokenToString;
    currentToken = symbolTable.end();
}

void Scanner::appendList(std::list<Token> &list)
{
    char ch;
    Operator op;
    Keyword k;
    Symbol s;
    if (end)
        return;
    if (inputFile.eof())
    {
        list.push_back(Token(TokenType::END, ""));
        end = true;
    }
    if (inputFile.get(ch) && !inputFile.eof())
    {
        if (isspace(ch) || ch == '\\')
        {
            do
            {
                if (ch == '\n')
                    lineNo++;
            } while (inputFile.get(ch) && isspace(ch) && !inputFile.eof());
            if (ch == '\n')
                lineNo++;
            if (inputFile.eof())
            {
                list.push_back(Token(TokenType::END, ""));
                end = true;
                return;
            }
        }

        if (ch == '\"')
        {
            handleStr(list);
            return;
        }

        else if (ch == '\'')
        {
            handleChar(list);
            return;
        }

        else if (ch == '#')
        {
            handleDirective();
            return;
        }

        if (ch == '/')
        {

            if (inputFile.peek() == '/' || inputFile.peek() == '*')
            {
                handleComment();
                appendList(list);
                return;
            }
        }
        if (s.isSymbol(ch))
        {

            std::string temp;
            temp.push_back(ch);
            list.push_back(Token(s(ch), temp));
        }
        else if (ch == '.')
        {
            char ch1, ch2;

            inputFile.get(ch1);
            if (isdigit(ch1))
            {
                std::string floatType = "0.";
                floatType.push_back(ch1);
                for (inputFile.get(ch1); !inputFile.eof() && isdigit(ch1); inputFile.get(ch1))
                    floatType.push_back(ch1);
                inputFile.unget();
                list.push_back(Token(TokenType::CONSTANT, floatType));
                return;
            }
            inputFile.get(ch2);
            if (ch1 == '.' && ch2 == '.')
                list.push_back(Token(TokenType::ELLIPSIS, "..."));
            else
            {
                inputFile.unget();
                inputFile.unget();
                list.push_back(Token(TokenType::DOT, "."));
            }
        }
        // Operators
        else if (op.beginOperator(ch))
        {

            std::string temp;
            temp.push_back(ch);
            char ch;
            if ((ch = inputFile.peek()) != '\n')
            {

                temp.push_back(ch);
                if (op.isOperator(temp))
                {
                    inputFile.get(ch);

                    if ((ch = inputFile.peek()) == '=')
                    {
                        temp.push_back(ch);
                        inputFile.get(ch);
                        if (temp != ">>=" && temp != "<<=")
                            loggedError.addError(lineNo, "unrecognized token");
                    }

                    list.push_back(Token(op(temp), temp));
                }

                else
                {
                    temp.pop_back();
                    list.push_back(Token(op(temp), temp));
                }
            }
            else

                list.push_back(Token(op(temp), temp));
        }
        else
        {

            std::string temp;
            temp.push_back(ch);
            char peeked = inputFile.peek();
            // Handling integer
            // Hex integer
            if (ch == '0' && (peeked == 'x' || peeked == 'X'))
            {
                inputFile.get(ch); // skip 'x'
                temp.push_back(ch);
                while ((ch = inputFile.peek()))
                {
                    if (isxdigit(ch))
                    {
                        temp.push_back(ch);
                        inputFile.get(ch);
                    }
                    else
                    {
                        if (ch == 'u' || ch == 'U')
                        {
                            temp.push_back(ch);
                            inputFile.get(ch);
                            char next = inputFile.peek();
                            if (next == 'l' || next == 'L')
                            {
                                temp.push_back(next);
                                inputFile.get(ch);
                            }
                        }
                        else if (ch == 'l')
                        {
                            temp.push_back(ch);
                            inputFile.get(ch);
                            char next = inputFile.peek();
                            if (next == 'l' || next == 'u' || next == 'U')
                            {
                                temp.push_back(next);
                                inputFile.get(ch);
                            }
                        }
                        else if (ch == 'L')
                        {
                            temp.push_back(ch);
                            inputFile.get(ch);
                            char next = inputFile.peek();

                            if (next == 'L' || next == 'u' || next == 'U')
                            {
                                temp.push_back(next);
                                inputFile.get(ch);
                            }
                        }
                        list.push_back(Token(TokenType::CONSTANT, temp));
                        break;
                    }
                }
            }
            else if (isdigit(ch))
            {
                bool isFloat = false;
                while ((ch = inputFile.peek()))
                {
                    if (isdigit(ch))
                    {
                        temp.push_back(ch);
                        inputFile.get(ch);
                    }
                    else
                    {

                        if ((ch == '.' && isFloat == false) || ch == 'e')
                        {
                            temp.push_back(ch);
                            inputFile.get(ch);
                            if (ch == 'e')
                            {
                                inputFile.get(ch);
                                if (ch != '+')
                                    loggedError.addError(lineNo, "float type error");
                                temp.push_back(ch);
                            }
                            isFloat = true;
                        }
                        else
                        {
                            if (isFloat && (ch == 'f' || ch == 'F' || ch == 'l' || ch == 'L'))
                            {
                                inputFile.get(ch);
                                temp.push_back(ch);
                            }
                            else if (!isFloat && (ch == 'u' || ch == 'U' || ch == 'l' || ch == 'L'))
                            {
                                if (ch == 'u' || ch == 'U')
                                {
                                    temp.push_back(ch);
                                    inputFile.get(ch);
                                    char next = inputFile.peek();
                                    if (next == 'l' || next == 'L')
                                    {
                                        temp.push_back(next);
                                        inputFile.get(ch);
                                    }
                                }
                                else if (ch == 'l')
                                {
                                    temp.push_back(ch);
                                    inputFile.get(ch);
                                    char next = inputFile.peek();
                                    if (next == 'l' || next == 'u' || next == 'U')
                                    {
                                        temp.push_back(next);
                                        inputFile.get(ch);
                                    }
                                }
                                else if (ch == 'L')
                                {
                                    temp.push_back(ch);
                                    inputFile.get(ch);
                                    char next = inputFile.peek();

                                    if (next == 'L' || next == 'u' || next == 'U')
                                    {
                                        temp.push_back(next);
                                        inputFile.get(ch);
                                    }
                                }
                            }
                            Token t;
                            t.lexeme = temp;

                            t.type = TokenType::CONSTANT;
                            list.push_back(t);
                            break;
                        }
                    }
                }
            }
            else
            {

                while (inputFile.get(ch))
                {
                    if (s.isSymbol(ch) || op.beginOperator(ch) || isspace(ch))
                    {
                        Token t;
                        if (ch == '\n')
                            lineNo++;
                        inputFile.unget();

                        if (k.isKeyword(temp))
                        {
                            t.lexeme = temp;
                            t.type = k(temp);
                            list.push_back(t);
                        }
                        else
                        {
                            char peeked = inputFile.peek();
                            if (isDefinedMacro(temp) && (op.beginOperator(peeked) || s.isSymbol(peeked) || isspace(peeked)))
                            {
                                auto macro = getDefinedMacro(temp);
                                while (isspace(ch))
                                {
                                    if (ch == '\n')
                                        lineNo++;
                                    inputFile.get(ch);
                                }
                                if (ch != '(')
                                    list.insert(list.end(), macro->second.tokens.begin(), macro->second.tokens.end());
                                else
                                {
                                    std::list<Token> parameter;
                                    while (char checkChar = inputFile.get())
                                    {
                                        if (checkChar == ')')
                                            break;
                                        if (checkChar == ',')
                                        {
                                            if (inputFile.peek() != ' ')
                                                appendList(parameter);
                                            continue;
                                        }

                                        appendList(parameter);
                                    }
                                    auto itr = parameter.begin();
                                    std::map<std::string, Token> paraMap;
                                    for (int i = 0; i < macro->second.parameters.size(); i++)
                                    {
                                        paraMap.insert({macro->second.parameters[i], std::move(*itr)});
                                        ++itr;
                                    }
                                    for (auto begin = macro->second.tokens.begin(); begin != macro->second.tokens.end(); begin++)
                                    {
                                        auto found = paraMap.find(begin->lexeme);
                                        if (begin->type == TokenType::ID && found != paraMap.end())
                                        {
                                            list.push_back(found->second);
                                        }
                                        else
                                        {
                                            list.push_back(*begin);
                                        }
                                    }
                                }
                            }
                            else
                            {
                                t.type = TokenType::ID;
                                t.lexeme = temp;
                                list.push_back(t);
                            }
                        }

                        break;
                    }
                    else
                        temp.push_back(ch);
                }
            }
        }
    }
}
void Scanner::appendMacro(std::list<Token> &list)
{
    char ch;
    Operator op;
    Keyword k;
    Symbol s;
    if (end)
        return;
    if (inputFile.eof())
    {
        list.push_back(Token(TokenType::END, ""));
        end = true;
    }
    if (inputFile.get(ch) && !inputFile.eof())
    {
        if (isspace(ch) || ch == '\\')
        {
            do
            {
                if (ch == '\n')
                    lineNo++;
            } while (inputFile.get(ch) && isspace(ch) && !inputFile.eof());
            if (ch == '\n')
                lineNo++;
            if (inputFile.eof())
            {
                list.push_back(Token(TokenType::END, ""));
                end = true;
                return;
            }
        }

        if (ch == '\"')
        {
            handleStr(list);
            return;
        }

        else if (ch == '\'')
        {
            handleChar(list);
            return;
        }

        else if (ch == '#')
        {
            handleDirective();
            return;
        }

        if (ch == '/')
        {

            if (inputFile.peek() == '/' || inputFile.peek() == '*')
            {
                handleComment();
                appendList(list);
                return;
            }
        }
        if (s.isSymbol(ch))
        {

            std::string temp;
            temp.push_back(ch);
            list.push_back(Token(s(ch), temp));
        }
        else if (ch == '.')
        {
            char ch1, ch2;

            inputFile.get(ch1);
            if (isdigit(ch1))
            {
                std::string floatType = "0.";
                floatType.push_back(ch1);
                for (inputFile.get(ch1); !inputFile.eof() && isdigit(ch1); inputFile.get(ch1))
                    floatType.push_back(ch1);
                inputFile.unget();
                list.push_back(Token(TokenType::CONSTANT, floatType));
                return;
            }
            inputFile.get(ch2);
            if (ch1 == '.' && ch2 == '.')
                list.push_back(Token(TokenType::ELLIPSIS, "..."));
            else
            {
                inputFile.unget();
                inputFile.unget();
                list.push_back(Token(TokenType::DOT, "."));
            }
        }
        // Operators
        else if (op.beginOperator(ch))
        {

            std::string temp;
            temp.push_back(ch);
            char ch;
            if ((ch = inputFile.peek()) != '\n')
            {

                temp.push_back(ch);
                if (op.isOperator(temp))
                {
                    inputFile.get(ch);

                    if ((ch = inputFile.peek()) == '=')
                    {
                        temp.push_back(ch);
                        inputFile.get(ch);
                        if (temp != ">>=" && temp != "<<=")
                            loggedError.addError(lineNo, "unrecognized token");
                    }

                    list.push_back(Token(op(temp), temp));
                }

                else
                {
                    temp.pop_back();
                    list.push_back(Token(op(temp), temp));
                }
            }
            else

                list.push_back(Token(op(temp), temp));
        }
        else
        {

            std::string temp;
            temp.push_back(ch);
            char peeked = inputFile.peek();
            // Handling integer
            // Hex integer
            if (ch == '0' && (peeked == 'x' || peeked == 'X'))
            {
                inputFile.get(ch); // skip 'x'
                temp.push_back(ch);
                while ((ch = inputFile.peek()))
                {
                    if (isxdigit(ch))
                    {
                        temp.push_back(ch);
                        inputFile.get(ch);
                    }
                    else
                    {
                        if (ch == 'u' || ch == 'U')
                        {
                            temp.push_back(ch);
                            inputFile.get(ch);
                            char next = inputFile.peek();
                            if (next == 'l' || next == 'L')
                            {
                                temp.push_back(next);
                                inputFile.get(ch);
                            }
                        }
                        else if (ch == 'l')
                        {
                            temp.push_back(ch);
                            inputFile.get(ch);
                            char next = inputFile.peek();
                            if (next == 'l' || next == 'u' || next == 'U')
                            {
                                temp.push_back(next);
                                inputFile.get(ch);
                            }
                        }
                        else if (ch == 'L')
                        {
                            temp.push_back(ch);
                            inputFile.get(ch);
                            char next = inputFile.peek();

                            if (next == 'L' || next == 'u' || next == 'U')
                            {
                                temp.push_back(next);
                                inputFile.get(ch);
                            }
                        }
                        list.push_back(Token(TokenType::CONSTANT, temp));
                        break;
                    }
                }
            }
            else if (isdigit(ch))
            {
                bool isFloat = false;
                while ((ch = inputFile.peek()))
                {
                    if (isdigit(ch))
                    {
                        temp.push_back(ch);
                        inputFile.get(ch);
                    }
                    else
                    {

                        if ((ch == '.' && isFloat == false) || ch == 'e')
                        {
                            temp.push_back(ch);
                            inputFile.get(ch);
                            if (ch == 'e')
                            {
                                inputFile.get(ch);
                                if (ch != '+')
                                    loggedError.addError(lineNo, "float type error");
                                temp.push_back(ch);
                            }
                            isFloat = true;
                        }
                        else
                        {
                            if (isFloat && (ch == 'f' || ch == 'F' || ch == 'l' || ch == 'L'))
                            {
                                inputFile.get(ch);
                                temp.push_back(ch);
                            }
                            else if (!isFloat && (ch == 'u' || ch == 'U' || ch == 'l' || ch == 'L'))
                            {
                                if (ch == 'u' || ch == 'U')
                                {
                                    temp.push_back(ch);
                                    inputFile.get(ch);
                                    char next = inputFile.peek();
                                    if (next == 'l' || next == 'L')
                                    {
                                        temp.push_back(next);
                                        inputFile.get(ch);
                                    }
                                }
                                else if (ch == 'l')
                                {
                                    temp.push_back(ch);
                                    inputFile.get(ch);
                                    char next = inputFile.peek();
                                    if (next == 'l' || next == 'u' || next == 'U')
                                    {
                                        temp.push_back(next);
                                        inputFile.get(ch);
                                    }
                                }
                                else if (ch == 'L')
                                {
                                    temp.push_back(ch);
                                    inputFile.get(ch);
                                    char next = inputFile.peek();

                                    if (next == 'L' || next == 'u' || next == 'U')
                                    {
                                        temp.push_back(next);
                                        inputFile.get(ch);
                                    }
                                }
                            }
                            Token t;
                            t.lexeme = temp;

                            t.type = TokenType::CONSTANT;
                            list.push_back(t);
                            break;
                        }
                    }
                }
            }
            else
            {

                while (inputFile.get(ch))
                {
                    if (s.isSymbol(ch) || op.beginOperator(ch) || isspace(ch))
                    {
                        Token t;
                        if (ch == '\n')
                            lineNo++;
                        inputFile.unget();

                        if (k.isKeyword(temp))
                        {
                            t.lexeme = temp;
                            t.type = k(temp);
                            list.push_back(t);
                        }
                        else
                        {

                            t.type = TokenType::ID;
                            t.lexeme = temp;
                            list.push_back(t);
                        }

                        break;
                    }
                    else
                        temp.push_back(ch);
                }
            }
        }
    }
}

std::list<Token>::iterator Scanner::getNextToken()
{

    if (!end)
        appendList(symbolTable);
    if (symbolTable.empty())
        return symbolTable.end();

    if (currentToken == symbolTable.end())
    {
        currentToken = symbolTable.begin();
        return currentToken;
    }

    auto next = std::next(currentToken);
    while (next == symbolTable.end() && !end)
    {
        appendList(symbolTable);
        next = std::next(currentToken);
    }

    if (next == symbolTable.end())
        return currentToken;

    currentToken = next;
    return currentToken;
}
std::list<Token>::iterator Scanner::peekNextToken()
{
    if (!end)
        appendList(symbolTable);
    if (symbolTable.empty())
        return symbolTable.end();

    if (currentToken == symbolTable.end())
        return symbolTable.begin();

    auto next = std::next(currentToken);
    while (next == symbolTable.end() && !end)
    {
        appendList(symbolTable);
        next = std::next(currentToken);
    }

    return next;
}
std::list<Token>::iterator Scanner::peekPrevToken()
{
    if (currentToken == symbolTable.begin())
        return currentToken;
    return std::prev(currentToken);
}

std::list<Token>::iterator Scanner::ungetToken()
{
    if (currentToken == symbolTable.begin())
        return currentToken;
    currentToken = std::prev(currentToken);
    return currentToken;
}

void Scanner::handleStr(std::list<Token> &list)
{

    Token t;
    t.type = TokenType::STRING_LITERAL;
    // t.lexeme.push_back('\"');
    //  int thisLine = lineNo;
    char ch;
    while (inputFile.get(ch) && !inputFile.eof())
    {
        if (ch == '\\')
        {
            inputFile.get(ch);
            char escape = handleEscape(ch);
            if (isspace(ch))
            {
                if (ch != '\n')
                    loggedError.addError(lineNo, ESCAPE_ERROR);
                else
                    lineNo++;
                continue;
            }
            if (ch == escape && ch != '\?')
                loggedError.addError(lineNo, ESCAPE_ERROR);
            else
                t.lexeme.push_back(escape);
        }
        else if (ch == '\"')
        {
            // t.lexeme.push_back('\"');
            list.push_back(t);
            break;
        }
        else if (ch == '\n')
        {

            loggedError.addError(lineNo, STRING_ERROR);

            lineNo++;
            break;
        }
        else
            t.lexeme.push_back(ch);
    }
    if (inputFile.eof())
        loggedError.addError(lineNo, STRING_ERROR);
}
void Scanner::handleChar(std::list<Token> &list)
{

    Token t;
    t.type = TokenType::CONSTANT;
    // int thisLine = lineNo;
    t.lexeme.push_back('\'');
    char ch;
    inputFile.get(ch);
    if (ch == '\n')
    {
        loggedError.addError(lineNo++, ESCAPE_ERROR);
        // symbolTable.push_back(Token(TokenType::NEWLINE, ""));
    }
    else if (ch == '\\')
    {
        inputFile.get(ch);
        char escape = handleEscape(ch);
        if (ch == escape && ch != '\?')
            loggedError.addError(lineNo, ESCAPE_ERROR);
        else
            t.lexeme.push_back(escape);
        if (inputFile.peek() == '\'')
        {
            inputFile.get(ch);
            t.lexeme.push_back('\'');
            list.push_back(t);
            // symbolTable.push_back(Token(TokenType::SINGLE_QUOTE, "\'"));
        }
        else
            loggedError.addError(lineNo, CHAR_ERROR);
    }
    else
    {
        t.lexeme.push_back(ch);

        if (inputFile.peek() == '\'')
        {
            t.lexeme.push_back('\'');
            inputFile.get(ch);
        }
        else

            loggedError.addError(lineNo, CHAR_ERROR);

        list.push_back(t);
    }
}
void Scanner::handleDirective()
{
    std::string temp;
    char ch;
    Directive d;
    Token t;

    while (inputFile.get(ch))
    {
        if (ch == '\n')
        {
            lineNo++;
            // inputFile.unget();
            appendList(symbolTable);
            return;
        }
        if (!isspace(ch))
            break;
    }

    do
    {
        // if (ch == '\"' || ch == '<')
        //     break;
        temp.push_back(ch);
        inputFile.get(ch);
    } while (!isspace(ch) && ch != '<' && ch != '\"');
    if (ch == '\n')
        lineNo++;
    if (d.isDirective(temp))
    {
        TokenType type = d(temp);
        temp.clear();
        if (type == TokenType::INCLUDE)
            symbolTable.push_back(Token(type, ""));
        if (type == TokenType::INCLUDE)
        {
            while (isspace(ch) && ch != '\n')
                inputFile.get(ch);
            if (ch == '\n')
            {
                lineNo++;
                // inputFile.unget();
                loggedError.addError(lineNo, INCLUD_ERROR);
                return;
            }
            if (ch == '<')
            {
                t.type = TokenType::LT;
                symbolTable.push_back(t);
                while (inputFile.get(ch) && !inputFile.eof())
                {
                    if (ch == '>')
                        break;
                    if (isspace(ch))
                    {
                        inputFile.unget();
                        loggedError.addError(lineNo, INCLUD_ERROR);
                        return;
                    }
                    temp.push_back(ch);
                }
                if (inputFile.eof())
                {
                    loggedError.addError(lineNo, INCLUD_ERROR);
                    return;
                }
                t.type = TokenType::INCLUDE_PATH;
                t.lexeme = temp;
                symbolTable.push_back(t);
                t.type = TokenType::GT;
                t.lexeme.clear();
                symbolTable.push_back(t);
            }
            else if (ch == '\"')
            {
                t.type = TokenType::DOUBLE_QUOTE;
                symbolTable.push_back(t);

                while (inputFile.get(ch) && !inputFile.eof())
                {
                    if (ch == '\"')
                        break;
                    if (isspace(ch))
                    {
                        inputFile.unget();
                        loggedError.addError(lineNo, INCLUD_ERROR);
                        return;
                    }
                    temp.push_back(ch);
                }
                if (inputFile.eof())
                {
                    loggedError.addError(lineNo, INCLUD_ERROR);
                    return;
                }
                t.type = TokenType::INCLUDE_PATH;
                t.lexeme = temp;
                symbolTable.push_back(t);
                t.type = TokenType::DOUBLE_QUOTE;
                t.lexeme.clear();
                symbolTable.push_back(t);
            }
            else
                loggedError.addError(lineNo, INCLUD_ERROR);
        }

#define DEFI
#ifdef DEFI
        if (type == TokenType::DEFINE)
        {
            while (isspace(ch) && ch != '\n')
                inputFile.get(ch);
            if (ch == '\n')
            {
                lineNo++;
                loggedError.addError(lineNo, DEFINE_ERROR);
                appendList(symbolTable);
                return;
            }
            // inputFile.unget();
            while (!isspace(ch) && ch != '(')
            {
                temp.push_back(ch);
                inputFile.get(ch);
            }
            definedMacro.insert({temp, {}});
            auto itr = getDefinedMacro(temp);
            temp.clear();
            if (ch == '\n')
            {
                lineNo++;
                appendList(symbolTable);
                return;
            }

            else if (ch == '(')
            {
                while (ch != ')' && ch != '\n')
                {
                    inputFile.get(ch);
                    if (isspace(ch))
                        continue;
                    if (ch == ',')
                    {
                        itr->second.parameters.push_back(temp);
                        temp.clear();
                        continue;
                    }

                    temp.push_back(ch);
                }
                while (temp.back() == ')' || isspace(temp.back()))
                    temp.pop_back();
                itr->second.parameters.push_back(temp);
            }
            addToken(itr->second);
            appendList(symbolTable);
        }
#endif
    }

    // #endif
}
void Scanner::handleComment()
{
    char ch;
    inputFile.get(ch);
    if (ch == '/')
    {
        while (inputFile.get(ch))
        {
            if (ch == '\n')
            {
                ++lineNo;
                // Token t;
                // t.lexeme = '\0';
                // t.type = TokenType::NEWLINE;
                // symbolTable.push_back(t);
                return;
            }
        }
    }
    else // next ch is '*'
    {
        int thisLine = lineNo;
        while (inputFile.get(ch))
        {
            if (ch == '\n')
            {
                ++lineNo;
                // Token t;
                // t.lexeme = '\0';
                // t.type = TokenType::NEWLINE;
                // symbolTable.push_back(t);
                continue;
            }
            else
            {
                if (ch == '*')
                    if (inputFile.peek() == '/')
                    {
                        char waste;
                        inputFile.get(waste);
                        break;
                    }
            }
        }
        if (inputFile.eof())
            loggedError.addError(thisLine, COMMENT_ERROR);
    }
}

void Scanner::addToken(Macro &m)
{
    while (1)
    {
        char ch;
        inputFile.get(ch);
        if (ch == '\\')
        {
            inputFile.get(ch);
            if (ch != '\n')
            {
                loggedError.addError(lineNo, ESCAPE_ERROR);
                break;
            }
            else
                lineNo++;
        }
        else if (ch == '\n')
        {
            lineNo++;
            break;
        }
        else
        {
            inputFile.unget();
            appendMacro(m.tokens);
        }
    }
}