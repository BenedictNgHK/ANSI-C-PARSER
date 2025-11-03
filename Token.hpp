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
    
    // C11 keywords
    INLINE,
    RESTRICT,
    BOOL_TYPE,
    COMPLEX,
    IMAGINARY,
    ALIGNAS,
    ALIGNOF,
    ATOMIC,
    GENERIC,
    NORETURN,
    STATIC_ASSERT,
    THREAD_LOCAL,
    
    FUNC_NAME,

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
    GOTO,
    QUESTION,
    CARET,
    PIPE,

    // syntax error

    // END: EOF
    END,

    // Non-terminal symbols for parse tree
    INCLUDE_STMT,
    TRANSLATION_UNIT,
    EXTERNAL_DECLARATION,
    FUNCTION_DEFINITION,
    DECLARATION,
    DECLARATION_LIST,
    
    // Expressions
    PRIMARY_EXPRESSION,
    POSTFIX_EXPRESSION,
    ARGUMENT_EXPRESSION_LIST,
    UNARY_EXPRESSION,
    CAST_EXPRESSION,
    MULTIPLICATIVE_EXPRESSION,
    ADDITIVE_EXPRESSION,
    SHIFT_EXPRESSION,
    RELATIONAL_EXPRESSION,
    EQUALITY_EXPRESSION,
    AND_EXPRESSION,
    EXCLUSIVE_OR_EXPRESSION,
    INCLUSIVE_OR_EXPRESSION,
    LOGICAL_AND_EXPRESSION,
    LOGICAL_OR_EXPRESSION,
    CONDITIONAL_EXPRESSION,
    ASSIGNMENT_EXPRESSION,
    EXPRESSION,
    CONSTANT_EXPRESSION,
    
    // Declarations
    STRUCT_UNION_SPECIFIER,
    STRUCT_DECLARATION_LIST,
    SPECIFIER_QUALIFIER_LIST,
    STRUCT_DECLARATION,
    STRUCT_DECLARATOR,
    STRUCT_DECLARATOR_LIST,
    DECLARATOR,
    DIRECT_DECLARATOR,
    POINTER,
    PARAMETER_TYPE_LIST,
    PARAMETER_LIST,
    PARAMETER_DECLARATION,
    DECLARATION_SPECIFIERS,
    INIT_DECLARATOR,
    INIT_DECLARATOR_LIST,
    ABSTRACT_DECLARATOR,
    IDENTIFIER_LIST,
    DIRECT_ABSTRACT_DECLARATOR,
    TYPE_QUALIFIER_LIST,
    TYPE_NAME,
    INITIALIZER,
    INITIALIZER_LIST,
    DESIGNATION,
    DESIGNATOR_LIST,
    DESIGNATOR,
    ENUM_SPECIFIER,
    ENUMERATOR_LIST,
    ENUMERATOR,
    
    // Statements
    STATEMENT,
    LABELED_STATEMENT,
    COMPOUND_STATEMENT,
    BLOCK_ITEM_LIST,
    BLOCK_ITEM,
    EXPRESSION_STATEMENT,
    SELECTION_STATEMENT,
    ITERATION_STATEMENT,
    JUMP_STATEMENT,
    
    // C11 specific non-terminals
    GENERIC_SELECTION,
    GENERIC_ASSOC_LIST,
    GENERIC_ASSOCIATION,
    STATIC_ASSERT_DECLARATION,
    ALIGNMENT_SPECIFIER,
    ATOMIC_TYPE_SPECIFIER,
    FUNCTION_SPECIFIER,
    STORAGE_CLASS_SPECIFIER,
    TYPE_SPECIFIER,
    TYPE_QUALIFIER
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
    Token(TokenType type, const std::string &lexeme, int lineNo = 1);
    Token(TokenType type, const std::string &&lexeme, int lineNo = 1);
    inline bool isStmt(TokenType t)
    {
        switch (t)
        {
        case TokenType::TRANSLATION_UNIT:
        case TokenType::EXTERNAL_DECLARATION:
        case TokenType::FUNCTION_DEFINITION:
        case TokenType::DECLARATION:
        case TokenType::DECLARATION_LIST:
        case TokenType::INCLUDE_STMT:
        case TokenType::PRIMARY_EXPRESSION:
        case TokenType::POSTFIX_EXPRESSION:
        case TokenType::ARGUMENT_EXPRESSION_LIST:
        case TokenType::UNARY_EXPRESSION:
        case TokenType::CAST_EXPRESSION:
        case TokenType::MULTIPLICATIVE_EXPRESSION:
        case TokenType::ADDITIVE_EXPRESSION:
        case TokenType::SHIFT_EXPRESSION:
        case TokenType::RELATIONAL_EXPRESSION:
        case TokenType::EQUALITY_EXPRESSION:
        case TokenType::AND_EXPRESSION:
        case TokenType::EXCLUSIVE_OR_EXPRESSION:
        case TokenType::INCLUSIVE_OR_EXPRESSION:
        case TokenType::LOGICAL_AND_EXPRESSION:
        case TokenType::LOGICAL_OR_EXPRESSION:
        case TokenType::CONDITIONAL_EXPRESSION:
        case TokenType::ASSIGNMENT_EXPRESSION:
        case TokenType::EXPRESSION:
        case TokenType::CONSTANT_EXPRESSION:
        case TokenType::STRUCT_UNION_SPECIFIER:
        case TokenType::STRUCT_DECLARATION_LIST:
        case TokenType::SPECIFIER_QUALIFIER_LIST:
        case TokenType::STRUCT_DECLARATION:
        case TokenType::STRUCT_DECLARATOR:
        case TokenType::STRUCT_DECLARATOR_LIST:
        case TokenType::DECLARATOR:
        case TokenType::DIRECT_DECLARATOR:
        case TokenType::POINTER:
        case TokenType::PARAMETER_TYPE_LIST:
        case TokenType::PARAMETER_LIST:
        case TokenType::PARAMETER_DECLARATION:
        case TokenType::DECLARATION_SPECIFIERS:
        case TokenType::INIT_DECLARATOR:
        case TokenType::INIT_DECLARATOR_LIST:
        case TokenType::ABSTRACT_DECLARATOR:
        case TokenType::IDENTIFIER_LIST:
        case TokenType::DIRECT_ABSTRACT_DECLARATOR:
        case TokenType::TYPE_QUALIFIER_LIST:
        case TokenType::TYPE_NAME:
        case TokenType::INITIALIZER:
        case TokenType::INITIALIZER_LIST:
        case TokenType::DESIGNATION:
        case TokenType::DESIGNATOR_LIST:
        case TokenType::DESIGNATOR:
        case TokenType::ENUM_SPECIFIER:
        case TokenType::ENUMERATOR_LIST:
        case TokenType::ENUMERATOR:
        case TokenType::STATEMENT:
        case TokenType::LABELED_STATEMENT:
        case TokenType::COMPOUND_STATEMENT:
        case TokenType::BLOCK_ITEM_LIST:
        case TokenType::BLOCK_ITEM:
        case TokenType::EXPRESSION_STATEMENT:
        case TokenType::SELECTION_STATEMENT:
        case TokenType::ITERATION_STATEMENT:
        case TokenType::JUMP_STATEMENT:
        case TokenType::GENERIC_SELECTION:
        case TokenType::GENERIC_ASSOC_LIST:
        case TokenType::GENERIC_ASSOCIATION:
        case TokenType::STATIC_ASSERT_DECLARATION:
        case TokenType::ALIGNMENT_SPECIFIER:
        case TokenType::ATOMIC_TYPE_SPECIFIER:
        case TokenType::FUNCTION_SPECIFIER:
        case TokenType::STORAGE_CLASS_SPECIFIER:
        case TokenType::TYPE_SPECIFIER:
        case TokenType::TYPE_QUALIFIER:
            return true;
        default:
            return false;
        }
    }
    TokenType type;
    int lineNo;
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