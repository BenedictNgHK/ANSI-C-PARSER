#include "Token.hpp"

const std::map<TokenType, std::string> TokenToString::table = {

    // Operators
    {TokenType::PLUS, "PLUS"},
    {TokenType::MINUS, "MINUS"},
    {TokenType::MUL, "MUL"},
    {TokenType::DIV, "DIV"},
    {TokenType::MOD, "MOD"},
    {TokenType::REFERENCE, "REFERENCE"},
    {TokenType::LT, "LT"},
    {TokenType::GT, "GT"},
    {TokenType::LTE, "LTE"},
    {TokenType::GTE, "GTE"},
    {TokenType::ASSIGN, "ASSIGN"},
    {TokenType::EQ, "EQ"},
    {TokenType::UNEQUAL, "UNEQUAL"},
    {TokenType::AND, "AND"},
    {TokenType::NOT, "NOT"},
    {TokenType::OR, "OR"},
    {TokenType::ARRORW, "ARROW"},
    {TokenType::DOT, "DOT"},
    {TokenType::INC, "INC"},
    {TokenType::DEC, "DEC"},
    {TokenType::TILDE, "TILDE"},
    {TokenType::ADD_ASSIGN, "ADD_ASSIGN"},
    {TokenType::SUB_ASSIGN, "SUB_ASSIGN"},
    {TokenType::MUL_ASSIGN, "MUL_ASSIGN"},
    {TokenType::DIV_ASSIGN, "DIV_ASSIGN"},
    {TokenType::MOD_ASSIGN, "MOD_ASSIGN"},
    {TokenType::AND_ASSIGN, "AND_ASSIGN"},
    {TokenType::OR_ASSIGN, "OR_ASSIGN"},
    {TokenType::XOR_ASSIGN, "XOR_ASSIGN"},
    {TokenType::LEFT_ASSIGN, "LEFT_ASSIGN"},
    {TokenType::RIGHT_ASSIGN, "RIGH,ASSIGN"},
    {TokenType::LEF_SHIFT, "LEFT_SHIFT"},
    {TokenType::RIGHT_SHIFT, "RIGHT_SHIFT"},
    {TokenType::DIRECT_DECLARATOR, "DIRECT_DECLARATOR"},
    {TokenType::POINTER, "POINTER"},
    {TokenType::ASSIGNMENT_EXPRESSION, "ASSIGNMENT_EXPRESSION"},
    {TokenType::PARAMETER_TYPE_LIST, "PARAMETER_TYPE_LIST"},

    {TokenType::PARAMETER_LIST, "PARAMETER_LIST"},
    {TokenType::PARAMETER_DECLARATION, "PARAMETER_DECLARATION"},

    {TokenType::DECLARATION_SPECIFIERS, "DECLARATION_SPECIFIERS"},
    {TokenType::ABSTRACT_DECLARATOR, "ABSTRACT_DECLARATOR"},
    {TokenType::IDENTIFIER_LIST, "IDENTIFIER_LIST"},
    {TokenType::DIRECT_ABSTRACT_DECLARATOR, "DIRECT_ABSTRACT_DECLARATOR"},
    {TokenType::TYPE_QUALIFIER_LIST, "TYPE_QUALIFIER_LIST"},
    {TokenType::ELLIPSIS, "ELLIPSIS"},
    {TokenType::ID, "ID"},
    {TokenType::INITIALIZER, "INITIALIZER"},
    {TokenType::BACKSLASH, "BACKSLASH"},
    {TokenType::GOTO, "GOTO"},
    {TokenType::QUESTION, "QUESTION"},
    {TokenType::CARET, "CARET"},
    {TokenType::PIPE, "PIPE"},

    // Keyword
    {TokenType::DO, "DO"},
    {TokenType::AUTO, "AUTO"},
    {TokenType::REGISTER, "REGISTER"},
    {TokenType::ENUM, "ENUM"},
    {TokenType::SIGNED, "SIGNED"},
    {TokenType::WHILE, "WHILE"},
    {TokenType::FOR, "FOR"},
    {TokenType::BRK, "BRK"},
    {TokenType::CONT, "CONT"},
    {TokenType::SWITCH, "SWITCH"},
    {TokenType::CASE, "CASE"},
    {TokenType::DEFAULT, "DEFAULT"},
    {TokenType::EXTERN, "EXTERN"},
    {TokenType::IF, "IF"},
    {TokenType::ELSE, "ELSE"},

    {TokenType::INT_TYPE, "INT_TYPE"},
    {TokenType::CHAR_TYPE, "CHAR_TYPE"},
    {TokenType::LONG_TYPE, "LONG_TYPE"},
    {TokenType::SHORT_TYPE, "SHORT_TYPE"},
    {TokenType::FLOAT_TYPE, "FLOAT_TYPE"},
    {TokenType::DOULBLE_TYPE, "DOUBLE_TYPE"},
    {TokenType::VOID, "VOID"},
    {TokenType::MAIN, "MAIN"},
    {TokenType::RETURN, "RETURN"},
    {TokenType::SIZEOF, "SIZEOF"},
    {TokenType::STRUCT, "STRUCT"},
    {TokenType::UNION, "UNION"},
    {TokenType::TYPEDEF, "TYPEDEF"},
    {TokenType::STATIC, "STATIC"},
    {TokenType::VOLATILE, "VOLATILE"},
    {TokenType::CONST, "CONST"},
    {TokenType::UNSINGED, "UNSINGED"},
    {TokenType::ENUM, "ENUM"},
    
    // C11 keywords
    {TokenType::INLINE, "INLINE"},
    {TokenType::RESTRICT, "RESTRICT"},
    {TokenType::BOOL_TYPE, "BOOL_TYPE"},
    {TokenType::COMPLEX, "COMPLEX"},
    {TokenType::IMAGINARY, "IMAGINARY"},
    {TokenType::ALIGNAS, "ALIGNAS"},
    {TokenType::ALIGNOF, "ALIGNOF"},
    {TokenType::ATOMIC, "ATOMIC"},
    {TokenType::GENERIC, "GENERIC"},
    {TokenType::NORETURN, "NORETURN"},
    {TokenType::STATIC_ASSERT, "STATIC_ASSERT"},
    {TokenType::THREAD_LOCAL, "THREAD_LOCAL"},
    {TokenType::FUNC_NAME, "FUNC_NAME"},

    // {TokenType::INT, "INT"},
    // {TokenType::CHAR, "CHAR"},
    {TokenType::STRING_LITERAL, "STRING_LITERAL"},
    // {TokenType::FLOAT, "FLOAT"},
    {TokenType::CONSTANT, "CONSTANT"},

    // Invisible characters
    // {TokenType::NEWLINE, "NEWLINE"},
    // {TokenType::SPACE, "SPACE"},
    // {TokenType::TAB, "TAB"},

    // Directives
    {TokenType::INCLUDE, "INCLUDE"},
    {TokenType::IFDEF, "IFDEF"},
    {TokenType::IFNDEF, "IFNDEF"},
    {TokenType::ENDIF, "ENDIF"},
    {TokenType::DEFINE, "DEFINE"},
    {TokenType::UNDEF, "UNDEF"},
    {TokenType::DEFINED, "DEFINED"},
    {TokenType::IF_DIRECTIVE, "IF_DIRECTIVE"},
    {TokenType::ELIF_DIRECTIVE, "ELIF_DIRECTIVE"},
    {TokenType::INCLUDE_PATH, "INCLUDE_PATH"},

    // Symbols
    {TokenType::L_CUR, "L_CUR"},           // {
    {TokenType::R_CUR, "R_CUR"},           // }
    {TokenType::L_SQR, "L_SQR"},           // [
    {TokenType::R_SQR, "R_SQR"},           // ]
    {TokenType::L_BR, "L_BR"},             // (
    {TokenType::R_BR, "R_BR"},             // )
    {TokenType::COMMA, "COMMA"},           // ,
    {TokenType::COLON, "COLON"},           // :
    {TokenType::SEMI_COLON, "SEMI_COLON"}, // ;
    {TokenType::DOUBLE_QUOTE, "DOUBLE_QUOTE"},
    {TokenType::SINGLE_QUOTE, "SINGLE_QUOTE"},

    // END: EOF
    {TokenType::END, "END"},

    // non-terminal symbols
    {TokenType::TRANSLATION_UNIT, "TRANSLATION_UNIT"},
    {TokenType::EXTERNAL_DECLARATION, "EXTERNAL_DECLARATION"},
    {TokenType::FUNCTION_DEFINITION, "FUNCTION_DEFINITION"},
    {TokenType::DECLARATION, "DECLARATION"},
    {TokenType::DECLARATION_LIST, "DECLARATION_LIST"},
    {TokenType::INCLUDE_STMT, "INCLUDE_STMT"},
    
    // Expressions
    {TokenType::PRIMARY_EXPRESSION, "PRIMARY_EXPRESSION"},
    {TokenType::POSTFIX_EXPRESSION, "POSTFIX_EXPRESSION"},
    {TokenType::ARGUMENT_EXPRESSION_LIST, "ARGUMENT_EXPRESSION_LIST"},
    {TokenType::UNARY_EXPRESSION, "UNARY_EXPRESSION"},
    {TokenType::CAST_EXPRESSION, "CAST_EXPRESSION"},
    {TokenType::MULTIPLICATIVE_EXPRESSION, "MULTIPLICATIVE_EXPRESSION"},
    {TokenType::ADDITIVE_EXPRESSION, "ADDITIVE_EXPRESSION"},
    {TokenType::SHIFT_EXPRESSION, "SHIFT_EXPRESSION"},
    {TokenType::RELATIONAL_EXPRESSION, "RELATIONAL_EXPRESSION"},
    {TokenType::EQUALITY_EXPRESSION, "EQUALITY_EXPRESSION"},
    {TokenType::AND_EXPRESSION, "AND_EXPRESSION"},
    {TokenType::EXCLUSIVE_OR_EXPRESSION, "EXCLUSIVE_OR_EXPRESSION"},
    {TokenType::INCLUSIVE_OR_EXPRESSION, "INCLUSIVE_OR_EXPRESSION"},
    {TokenType::LOGICAL_AND_EXPRESSION, "LOGICAL_AND_EXPRESSION"},
    {TokenType::LOGICAL_OR_EXPRESSION, "LOGICAL_OR_EXPRESSION"},
    {TokenType::CONDITIONAL_EXPRESSION, "CONDITIONAL_EXPRESSION"},
    {TokenType::ASSIGNMENT_EXPRESSION, "ASSIGNMENT_EXPRESSION"},
    {TokenType::EXPRESSION, "EXPRESSION"},
    {TokenType::CONSTANT_EXPRESSION, "CONSTANT_EXPRESSION"},
    
    // Declarations
    {TokenType::STRUCT_UNION_SPECIFIER, "STRUCT_UNION_SPECIFIER"},
    {TokenType::STRUCT_DECLARATION_LIST, "STRUCT_DECLARATION_LIST"},
    {TokenType::SPECIFIER_QUALIFIER_LIST, "SPECIFIER_QUALIFIER_LIST"},
    {TokenType::STRUCT_DECLARATION, "STRUCT_DECLARATION"},
    {TokenType::STRUCT_DECLARATOR, "STRUCT_DECLARATOR"},
    {TokenType::STRUCT_DECLARATOR_LIST, "STRUCT_DECLARATOR_LIST"},
    {TokenType::DECLARATOR, "DECLARATOR"},
    {TokenType::DIRECT_DECLARATOR, "DIRECT_DECLARATOR"},
    {TokenType::POINTER, "POINTER"},
    {TokenType::PARAMETER_TYPE_LIST, "PARAMETER_TYPE_LIST"},
    {TokenType::PARAMETER_LIST, "PARAMETER_LIST"},
    {TokenType::PARAMETER_DECLARATION, "PARAMETER_DECLARATION"},
    {TokenType::DECLARATION_SPECIFIERS, "DECLARATION_SPECIFIERS"},
    {TokenType::INIT_DECLARATOR, "INIT_DECLARATOR"},
    {TokenType::INIT_DECLARATOR_LIST, "INIT_DECLARATOR_LIST"},
    {TokenType::ABSTRACT_DECLARATOR, "ABSTRACT_DECLARATOR"},
    {TokenType::IDENTIFIER_LIST, "IDENTIFIER_LIST"},
    {TokenType::DIRECT_ABSTRACT_DECLARATOR, "DIRECT_ABSTRACT_DECLARATOR"},
    {TokenType::TYPE_QUALIFIER_LIST, "TYPE_QUALIFIER_LIST"},
    {TokenType::TYPE_NAME, "TYPE_NAME"},
    {TokenType::INITIALIZER, "INITIALIZER"},
    {TokenType::INITIALIZER_LIST, "INITIALIZER_LIST"},
    {TokenType::DESIGNATION, "DESIGNATION"},
    {TokenType::DESIGNATOR_LIST, "DESIGNATOR_LIST"},
    {TokenType::DESIGNATOR, "DESIGNATOR"},
    {TokenType::ENUM_SPECIFIER, "ENUM_SPECIFIER"},
    {TokenType::ENUMERATOR_LIST, "ENUMERATOR_LIST"},
    {TokenType::ENUMERATOR, "ENUMERATOR"},
    
    // Statements
    {TokenType::STATEMENT, "STATEMENT"},
    {TokenType::LABELED_STATEMENT, "LABELED_STATEMENT"},
    {TokenType::COMPOUND_STATEMENT, "COMPOUND_STATEMENT"},
    {TokenType::BLOCK_ITEM_LIST, "BLOCK_ITEM_LIST"},
    {TokenType::BLOCK_ITEM, "BLOCK_ITEM"},
    {TokenType::EXPRESSION_STATEMENT, "EXPRESSION_STATEMENT"},
    {TokenType::SELECTION_STATEMENT, "SELECTION_STATEMENT"},
    {TokenType::ITERATION_STATEMENT, "ITERATION_STATEMENT"},
    {TokenType::JUMP_STATEMENT, "JUMP_STATEMENT"},
    
    // C11 specific non-terminals
    {TokenType::GENERIC_SELECTION, "GENERIC_SELECTION"},
    {TokenType::GENERIC_ASSOC_LIST, "GENERIC_ASSOC_LIST"},
    {TokenType::GENERIC_ASSOCIATION, "GENERIC_ASSOCIATION"},
    {TokenType::STATIC_ASSERT_DECLARATION, "STATIC_ASSERT_DECLARATION"},
    {TokenType::ALIGNMENT_SPECIFIER, "ALIGNMENT_SPECIFIER"},
    {TokenType::ATOMIC_TYPE_SPECIFIER, "ATOMIC_TYPE_SPECIFIER"},
    {TokenType::FUNCTION_SPECIFIER, "FUNCTION_SPECIFIER"},
    {TokenType::STORAGE_CLASS_SPECIFIER, "STORAGE_CLASS_SPECIFIER"},
    {TokenType::TYPE_SPECIFIER, "TYPE_SPECIFIER"},
    {TokenType::TYPE_QUALIFIER, "TYPE_QUALIFIER"}};

std::string
TokenToString::operator()(const TokenType &t)
{
    auto itr = table.find(t);
    return itr->second;
}
std::string TokenToString::operator()(const TokenType &&t)
{
    auto itr = table.find(t);
    return itr->second;
}

Token::Token()
{
    memset(this, 0, sizeof(Token));
}
Token::Token(TokenType type, const std::string &lexeme) : type(type), lexeme(lexeme)
{
}

Token::Token(TokenType type, const std::string &&lexeme) : type(type), lexeme(lexeme)
{
}

// Directive
const std::map<std::string, TokenType> Directive::table = {
    {
        "include",
        TokenType::INCLUDE,
    },
    {
        "ifdef",
        TokenType::IFDEF,
    },
    {
        "ifndef",
        TokenType::IFNDEF,
    },
    {
        "endif",
        TokenType::ENDIF,
    },
    {
        "define",
        TokenType::DEFINE,
    },
    {"undef",
     TokenType::UNDEF},
    {"defined",
     TokenType::DEFINED},
    {"elif", TokenType::ELIF_DIRECTIVE},
    {"if", TokenType::IF_DIRECTIVE}};
TokenType Directive::operator()(const std::string &&s)
{
    auto itr = table.find(s);
    return itr->second;
}
TokenType Directive::operator()(const std::string &s)
{
    auto itr = table.find(s);
    return itr->second;
}
bool Directive::isDirective(const std::string &s)
{
    return table.count(s);
}
bool Directive::isDirective(const std::string &&s)
{
    return table.count(s);
}

const std::map<std::string, TokenType> Keyword::table = {
    {"do", TokenType::DO},
    {"while", TokenType::WHILE},
    {"for", TokenType::FOR},
    {"break", TokenType::BRK},
    {"continue", TokenType::CONT},
    {"switch", TokenType::SWITCH},
    {"case", TokenType::CASE},
    {"default", TokenType::DEFAULT},

    {"if", TokenType::IF},
    {"else", TokenType::ELSE},
    {"main", TokenType::MAIN},
    {"return", TokenType::RETURN},
    {"goto", TokenType::GOTO},
    // Type
    {"int", TokenType::INT_TYPE},
    {"char", TokenType::CHAR_TYPE},
    {"long", TokenType::LONG_TYPE},
    {"short", TokenType::SHORT_TYPE},
    {"float", TokenType::FLOAT_TYPE},
    {"double", TokenType::DOULBLE_TYPE},
    {"void", TokenType::VOID},

    {"typedef", TokenType::TYPEDEF},
    {"struct", TokenType::STRUCT},
    {"union", TokenType::UNION},
    {"sizeof", TokenType::SIZEOF},
    {"const", TokenType::CONST},
    {"static", TokenType::STATIC},
    {"volatile", TokenType::VOLATILE},
    {"unsigned", TokenType::UNSINGED},
    {"enum", TokenType::ENUM},
    {"signed", TokenType::SIGNED},
    {"register", TokenType::REGISTER},
    {"auto", TokenType::AUTO},
    {"extern", TokenType::EXTERN},
    
    // C11 keywords
    {"inline", TokenType::INLINE},
    {"_Inline", TokenType::INLINE},
    {"restrict", TokenType::RESTRICT},
    {"_Restrict", TokenType::RESTRICT},
    {"_Bool", TokenType::BOOL_TYPE},
    {"_Complex", TokenType::COMPLEX},
    {"_Imaginary", TokenType::IMAGINARY},
    {"_Alignas", TokenType::ALIGNAS},
    {"_Alignof", TokenType::ALIGNOF},
    {"_Atomic", TokenType::ATOMIC},
    {"_Generic", TokenType::GENERIC},
    {"_Noreturn", TokenType::NORETURN},
    {"_Static_assert", TokenType::STATIC_ASSERT},
    {"_Thread_local", TokenType::THREAD_LOCAL},
    {"__func__", TokenType::FUNC_NAME}};

TokenType Keyword::operator()(const std::string &s)
{
    auto itr = table.find(s);
    return itr->second;
}
TokenType Keyword::operator()(const std::string &&s)
{
    auto itr = table.find(s);
    return itr->second;
}
bool Keyword::isKeyword(const std::string &s)
{

    return table.count(s);
}
bool Keyword::isKeyword(const std::string &&s)
{

    return table.count(s);
}

const std::map<std::string, TokenType> Operator::table = {
    {"+", TokenType::PLUS},
    {"-", TokenType::MINUS},
    {"*", TokenType::MUL},

    {"/", TokenType::DIV},

    {"%", TokenType::MOD},
    {"&", TokenType::REFERENCE},
    {"<", TokenType::LT},
    {">", TokenType::GT},
    {".", TokenType::DOT},
    {"~", TokenType::TILDE},
    {"<=", TokenType::LTE},
    {">=", TokenType::GTE},
    {"=", TokenType::ASSIGN},
    {"==", TokenType::EQ},
    {"!=", TokenType::UNEQUAL},
    {"+=", TokenType::ADD_ASSIGN},
    {"-=", TokenType::SUB_ASSIGN},
    {"*=", TokenType::MUL_ASSIGN},
    {"/=", TokenType::DIV_ASSIGN},
    {"%=", TokenType::MOD_ASSIGN},
    {"&=", TokenType::AND_ASSIGN},
    {"|=", TokenType::OR_ASSIGN},
    {"^=", TokenType::XOR_ASSIGN},
    {"<<=", TokenType::LEFT_ASSIGN},
    {">>=", TokenType::RIGHT_ASSIGN},
    {"<<", TokenType::LEF_SHIFT},
    {">>", TokenType::RIGHT_SHIFT},
    {"!", TokenType::NOT},
    {"&&", TokenType::AND},
    {"||", TokenType::OR},
    {"->", TokenType::ARRORW},
    {"++", TokenType::INC},
    {"--", TokenType::DEC},
    {"?", TokenType::QUESTION},
    {"^", TokenType::CARET},
    {"|", TokenType::PIPE}};

TokenType Operator::operator()(const std::string &&s)
{
    auto itr = table.find(s);
    return itr->second;
}
TokenType Operator::operator()(const std::string &s)
{
    auto itr = table.find(s);
    return itr->second;
}
TokenType Operator::operator()(const char ch)
{
    std::string s(ch, 1);
    auto itr = table.find(s);
    return itr->second;
}
bool Operator::isOperator(const std::string &&s)
{

    return table.count(s);
}
bool Operator::isOperator(const std::string &s)
{

    return table.count(s);
}
bool Operator::isOperator(const char ch)
{
    std::string s;
    s.push_back(ch);
    return table.count(s);
}
bool Operator::isOperator(const char ch1, const char ch2)
{
    std::string s;
    s.push_back(ch1);
    s.push_back(ch2);
    return table.count(s);
}
bool Operator::beginOperator(const char ch)
{
    return (ch == '*' || ch == '+' || ch == '%' || ch == '-' || ch == '/' || ch == '<' || ch == '>' || ch == '!' || ch == '&' || ch == '=' || ch == '|' || ch == '.' || ch == '^' || ch == '?' || ch == '~');
}

const std::map<char, TokenType> Symbol::table = {
    {'{', TokenType::L_CUR},      // {
    {'}', TokenType::R_CUR},      // }
    {'[', TokenType::L_SQR},      // [
    {']', TokenType::R_SQR},      // ]
    {'(', TokenType::L_BR},       // (
    {')', TokenType::R_BR},       // )
    {',', TokenType::COMMA},      // ,
    {':', TokenType::COLON},      // :
    {';', TokenType::SEMI_COLON}, // ;
    {'\"', TokenType::DOUBLE_QUOTE},
    {'\'', TokenType::SINGLE_QUOTE}};

TokenType Symbol::operator()(const char ch)
{
    auto itr = table.find(ch);
    return itr->second;
}
TokenType Symbol::operator()(const std::string &s)
{
    auto itr = table.find(s[0]);
    return itr->second;
}

bool Symbol::isSymbol(const char ch)
{

    return table.count(ch);
}
bool Symbol::isSymbol(const std::string &s)
{

    return table.count(s[0]);
}
