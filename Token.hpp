#ifndef TOKEN_HPP
#define TOKEN_HPP
#include <map>
#include <string>
enum class TokenType
{ // operators: +-*/ %<><=>====...
    PLUS,
    MINUS,
    MUL,
    DIV,

    MOD,
    REFERENCE,
    LT,
    GT,
    LTE,
    GTE,
    ASSIGN,
    EQ,
    UNEQUAL,
    NOT,
    AND,
    OR,
    ARRORW,
    DOT,
    INC,
    DEC,
    TILDE,
    MUL_ASSIGN,
    DIV_ASSIGN,
    MOD_ASSIGN,
    ADD_ASSIGN,
    SUB_ASSIGN,
    LEFT_ASSIGN,
    RIGHT_ASSIGN,
    OR_ASSIGN,
    AND_ASSIGN,
    XOR_ASSIGN,
    LEF_SHIFT,
    RIGHT_SHIFT,

    ID, // for names of function, variable, array

    // keyword
    AUTO,
    REGISTER,
    ENUM,
    SIGNED,

    DO,
    WHILE,
    FOR, // the loops
    BRK,
    CONT,
    SWITCH,
    CASE,
    DEFAULT,
    IF,
    ELSE,
    INT_TYPE,
    FLOAT_TYPE,
    LONG_TYPE,
    SHORT_TYPE,
    CHAR_TYPE,
    DOULBLE_TYPE,
    VOID,
    MAIN, // main function
    RETURN,
    SIZEOF,
    STRUCT,
    UNION,
    TYPEDEF,
    STATIC,
    VOLATILE,
    CONST,
    UNSINGED,
    EXTERN,

    // int, char, String,
    // INT,
    // CHAR,
    STRING_LITERAL, // string
                    // FLOAT,
    CONSTANT,
    // invisible characters
    // NEWLINE,
    // SPACE,
    // TAB,

    // Directives
    INCLUDE,
    IFDEF,
    IFNDEF,
    ENDIF,
    DEFINE,
    DEFINED,
    UNDEF,
    INCLUDE_PATH,
    IF_DIRECTIVE,
    ELIF_DIRECTIVE,

    // Symbols
    L_CUR,        // {
    R_CUR,        // }
    L_SQR,        // [
    R_SQR,        // ]
    L_BR,         // (
    R_BR,         // )
    COMMA,        // ,
    COLON,        // :
    SEMI_COLON,   // ;
    DOUBLE_QUOTE, // "
    SINGLE_QUOTE, // '

    ELLIPSIS,
    BACKSLASH,

    // syntax error

    // END: EOF
    END,

    // Statements

    INCLUDE_STMT,
    TRANSLATION_UNIT,
    STRUCT_UNION_SPECIFIER,
    STRUCT_DECLARATION_LIST,
    SPECIFIER_QUALIFIER_LIST,
    STRUCT_DECLARATION,
    STRUCT_DECLARATOR,
    STRUCT_DECLARATOR_LIST,
    DECLARATOR,

    DIRECT_DECLARATOR,
    ASSIGNMENT_EXP,
    POINTER,
    PARAMETER_TYPE_LIST,
    PARAMETER_LIST,
    PARAMETER_DECLARATION,
    DECLARATION_SPECIFIERS,
    INIT_DECLARATOR,
    ABSTRACT_DECLARATOR,
    IDENTIFIER_LIST,
    DIRECT_ABSTRACT_DECLARATOR,
    TYPE_QUALIFIER_LIST,
    INITIALIZER,
    ENUM_SPECIFIER,
    ENUMERATOR_LIST,
    ENUMERATOR
};

// convert TokenType to std::string
class TokenToString
{
private:
    const static std::map<TokenType, std::string> table;

public:
    explicit TokenToString() = default;
    TokenToString(TokenToString &&t) = delete;
    TokenToString(TokenToString &t) = delete;
    std::string operator()(const TokenType &t);
    std::string operator()(const TokenType &&t);
    ~TokenToString() {};
};
struct Token
{
    Token();
    Token(TokenType type, const std::string &lexeme);
    Token(TokenType type, const std::string &&lexeme);
    inline bool isStmt(TokenType t)
    {
        switch (t)
        {
        case TokenType::TRANSLATION_UNIT:
            return true;
        case TokenType::INCLUDE_STMT:
            return true;
        case TokenType::STRUCT_UNION_SPECIFIER:
            return true;
        case TokenType::STRUCT_DECLARATION_LIST:
            return true;
        case TokenType::SPECIFIER_QUALIFIER_LIST:
            return true;
        case TokenType::STRUCT_DECLARATION:
            return true;
        case TokenType::STRUCT_DECLARATOR:
            return true;
        case TokenType::DECLARATOR:
            return true;
        case TokenType::STRUCT_DECLARATOR_LIST:
            return true;
        case TokenType::ENUM_SPECIFIER:
            return true;
        case TokenType::ENUMERATOR_LIST:
            return true;
        case TokenType::ENUMERATOR:
            return true;
        case TokenType::DIRECT_DECLARATOR:
            return true;
        case TokenType::ASSIGNMENT_EXP:
            return true;
        case TokenType::POINTER:
            return true;
        case TokenType::TYPE_QUALIFIER_LIST:
            return true;
        case TokenType::PARAMETER_TYPE_LIST:
            return true;
        case TokenType::PARAMETER_LIST:
            return true;
        case TokenType::PARAMETER_DECLARATION:
            return true;
        case TokenType::DECLARATION_SPECIFIERS:
            return true;
        case TokenType::ABSTRACT_DECLARATOR:
            return true;
        case TokenType::IDENTIFIER_LIST:
            return true;
        case TokenType::INITIALIZER:
            return true;
        case TokenType::DIRECT_ABSTRACT_DECLARATOR:
            return true;

        default:
            return false;
        }
    }
    TokenType type;
    ~Token() = default;
    std::string lexeme;
};

class Directive
{
private:
    const static std::map<std::string, TokenType> table;

public:
    explicit Directive() = default;
    TokenType operator()(const std::string &&s);
    TokenType operator()(const std::string &s);
    bool isDirective(const std::string &s);
    bool isDirective(const std::string &&s);

    friend class Scanner;
};

class Keyword
{
private:
    const static std::map<std::string, TokenType> table;

public:
    explicit Keyword() = default;
    TokenType operator()(const std::string &&s);
    TokenType operator()(const std::string &s);
    bool isKeyword(const std::string &s);
    bool isKeyword(const std::string &&s);

    friend class Scanner;
};

class Operator
{
private:
    const static std::map<std::string, TokenType> table;

public:
    explicit Operator() = default;
    TokenType operator()(const std::string &&s);
    TokenType operator()(const std::string &s);
    TokenType operator()(const char ch);
    bool isOperator(const std::string &&s);
    bool isOperator(const std::string &s);
    bool isOperator(const char ch);
    bool isOperator(const char ch1, const char ch2);
    bool beginOperator(const char ch);
};

class Symbol
{
private:
    const static std::map<char, TokenType> table;

public:
    explicit Symbol() = default;
    TokenType operator()(const char ch);
    TokenType operator()(const std::string &s);
    bool isSymbol(const char ch);
    bool isSymbol(const std::string &s);
};

#endif