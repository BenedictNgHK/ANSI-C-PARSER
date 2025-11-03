#ifndef AST_HPP
#define AST_HPP
#include <memory>
#include "Scanner.hpp"
#include "Error.hpp"
#include <queue>
#include <array>
#include <set>

struct Node
{
    Token t;
    std::vector<std::shared_ptr<Node>> children;
    // Node(Token &&t){s}
    Node(Token t) : t(t) {};
    Node(TokenType type) : t(type, "") {};
    Node() = default;
};
class AST : public Scanner
{

protected:
    std::shared_ptr<Node> root;
    std::set<std::string> definedTypeNames;
    inline bool isTypeName(const std::string &name) { return definedTypeNames.count(name) > 0; }
    std::shared_ptr<Node> parsingFile(std::shared_ptr<Node> root);
    std::shared_ptr<Node> includeStmt();

    // Translation unit and external declarations
    std::shared_ptr<Node> translationUnit();
    std::shared_ptr<Node> externalDeclaration();
    std::shared_ptr<Node> functionDefinition(std::list<Token>::iterator begin);
    std::shared_ptr<Node> declarationList(std::list<Token>::iterator begin);

    // Declarations
    std::shared_ptr<Node> declaration(std::list<Token>::iterator begin);
    std::shared_ptr<Node> declarationSpecifier(std::list<Token>::iterator begin);
    std::shared_ptr<Node> initDeclaratorList(std::list<Token>::iterator begin);
    std::shared_ptr<Node> initDeclarator(std::list<Token>::iterator begin);

    // Struct/Union/Enum
    std::shared_ptr<Node> structUnionSpecifier(std::list<Token>::iterator begin);
    std::shared_ptr<Node> structDeclarationList(std::list<Token>::iterator begin);
    std::shared_ptr<Node> structDeclaration(std::list<Token>::iterator begin);
    std::shared_ptr<Node> structDeclaratorList(std::list<Token>::iterator begin);
    std::shared_ptr<Node> structDeclarator(std::list<Token>::iterator begin);
    std::shared_ptr<Node> enumSpecifier(std::list<Token>::iterator begin);
    std::shared_ptr<Node> enumerator(std::list<Token>::iterator begin);
    std::shared_ptr<Node> enumeratorList(std::list<Token>::iterator begin);

    // Declarators
    std::shared_ptr<Node> declarator(std::list<Token>::iterator begin);
    std::shared_ptr<Node> directDeclarator(std::list<Token>::iterator begin);
    std::shared_ptr<Node> pointer(std::list<Token>::iterator begin);
    std::shared_ptr<Node> abstractDeclarator(std::list<Token>::iterator begin);
    std::shared_ptr<Node> directAbstractDeclarator(std::list<Token>::iterator begin);

    // Parameters and type names
    std::shared_ptr<Node> parameterTypeList(std::list<Token>::iterator begin);
    std::shared_ptr<Node> parameterList(std::list<Token>::iterator begin);
    std::shared_ptr<Node> parameterDeclaration(std::list<Token>::iterator begin);
    std::shared_ptr<Node> identifierList(std::list<Token>::iterator begin);
    std::shared_ptr<Node> typeName(std::list<Token>::iterator begin);
    std::shared_ptr<Node> typeQualifierList(std::list<Token>::iterator begin);
    std::shared_ptr<Node> specifierQualifierList(std::list<Token>::iterator begin);

    // Initializers
    std::shared_ptr<Node> initializer(std::list<Token>::iterator begin);
    std::shared_ptr<Node> initializerList(std::list<Token>::iterator begin);
    std::shared_ptr<Node> designation(std::list<Token>::iterator begin);
    std::shared_ptr<Node> designatorList(std::list<Token>::iterator begin);
    std::shared_ptr<Node> designator(std::list<Token>::iterator begin);

    // C11 specific constructs
    std::shared_ptr<Node> genericSelection(std::list<Token>::iterator begin);
    std::shared_ptr<Node> genericAssocList(std::list<Token>::iterator begin);
    std::shared_ptr<Node> genericAssociation(std::list<Token>::iterator begin);
    std::shared_ptr<Node> staticAssertDeclaration(std::list<Token>::iterator begin);
    std::shared_ptr<Node> alignmentSpecifier(std::list<Token>::iterator begin);
    std::shared_ptr<Node> atomicTypeSpecifier(std::list<Token>::iterator begin);
    
    // Expressions
    std::shared_ptr<Node> primaryExpression(std::list<Token>::iterator begin);
    std::shared_ptr<Node> postfixExpression(std::list<Token>::iterator begin);
    std::shared_ptr<Node> argumentExpressionList(std::list<Token>::iterator begin);
    std::shared_ptr<Node> unaryExpression(std::list<Token>::iterator begin);
    std::shared_ptr<Node> castExpression(std::list<Token>::iterator begin);
    std::shared_ptr<Node> multiplicativeExpression(std::list<Token>::iterator begin);
    std::shared_ptr<Node> additiveExpression(std::list<Token>::iterator begin);
    std::shared_ptr<Node> shiftExpression(std::list<Token>::iterator begin);
    std::shared_ptr<Node> relationalExpression(std::list<Token>::iterator begin);
    std::shared_ptr<Node> equalityExpression(std::list<Token>::iterator begin);
    std::shared_ptr<Node> andExpression(std::list<Token>::iterator begin);
    std::shared_ptr<Node> exclusiveOrExpression(std::list<Token>::iterator begin);
    std::shared_ptr<Node> inclusiveOrExpression(std::list<Token>::iterator begin);
    std::shared_ptr<Node> logicalAndExpression(std::list<Token>::iterator begin);
    std::shared_ptr<Node> logicalOrExpression(std::list<Token>::iterator begin);
    std::shared_ptr<Node> conditionalExpression(std::list<Token>::iterator begin);
    std::shared_ptr<Node> assignmentExpression(std::list<Token>::iterator begin);
    std::shared_ptr<Node> expression(std::list<Token>::iterator begin);
    std::shared_ptr<Node> constantExpression(std::list<Token>::iterator begin);

    // Statements
    std::shared_ptr<Node> statement(std::list<Token>::iterator begin);
    std::shared_ptr<Node> labeledStatement(std::list<Token>::iterator begin);
    std::shared_ptr<Node> compoundStatement(std::list<Token>::iterator begin);
    std::shared_ptr<Node> blockItemList(std::list<Token>::iterator begin);
    std::shared_ptr<Node> blockItem(std::list<Token>::iterator begin);
    std::shared_ptr<Node> expressionStatement(std::list<Token>::iterator begin);
    std::shared_ptr<Node> selectionStatement(std::list<Token>::iterator begin);
    std::shared_ptr<Node> iterationStatement(std::list<Token>::iterator begin);
    std::shared_ptr<Node> jumpStatement(std::list<Token>::iterator begin);
    inline bool typeSpecifier(const std::list<Token>::iterator &itr)
    {
        TokenType type = itr->type;
        switch (type)
        {
        case TokenType::INT_TYPE:
            return true;
        case TokenType::CHAR_TYPE:
            return true;
        case TokenType::DOULBLE_TYPE:
            return true;
        case TokenType::FLOAT_TYPE:
            return true;
        case TokenType::LONG_TYPE:
            return true;
        case TokenType::SHORT_TYPE:
            return true;
        case TokenType::VOID:
            return true;
        case TokenType::UNSINGED:
            return true;
        case TokenType::SIGNED:
            return true;

        case TokenType::STRUCT:
            return true;
        case TokenType::UNION:
            return true;
        case TokenType::ENUM:
            return true;
            
        // C11 type specifiers
        case TokenType::BOOL_TYPE:
            return true;
        case TokenType::COMPLEX:
            return true;
        case TokenType::IMAGINARY:
            return true;
        case TokenType::ATOMIC:
            return true;

        default:
            if (!itr->lexeme.empty() && isTypeName(itr->lexeme))
                return true;
            return false;
        }
    }

    inline bool structUnion(const std::list<Token>::iterator &itr)
    {
        return itr->type == TokenType::STRUCT || itr->type == TokenType::UNION;
    }
    inline bool typeQualifier(const std::list<Token>::iterator &itr)
    {
        TokenType type = itr->type;
        switch (type)
        {
        case TokenType::CONST:
            return true;
        case TokenType::VOLATILE:
            return true;
        case TokenType::RESTRICT:
            return true;
        case TokenType::ATOMIC:
            return true;
        default:
            return false;
        }
    }
    inline bool storageClassSpecifier(const std::list<Token>::iterator &itr)
    {
        TokenType type = itr->type;
        switch (type)
        {
        case TokenType::TYPEDEF:
            return true;
        case TokenType::EXTERN:
            return true;
        case TokenType::AUTO:
            return true;
        case TokenType::STATIC:
            return true;
        case TokenType::REGISTER:
            return true;
        case TokenType::THREAD_LOCAL:
            return true;
        default:
            return false;
        }
    }
    
    inline bool functionSpecifier(const std::list<Token>::iterator &itr)
    {
        TokenType type = itr->type;
        switch (type)
        {
        case TokenType::INLINE:
            return true;
        case TokenType::NORETURN:
            return true;
        default:
            return false;
        }
    }
    
    inline bool isAlignmentSpecifier(const std::list<Token>::iterator &itr)
    {
        return itr->type == TokenType::ALIGNAS;
    }
    inline bool assignOperator(const std::list<Token>::iterator &itr)
    {
        switch (itr->type)
        {
        case TokenType::ASSIGN:
            return true;

        case TokenType::MUL_ASSIGN:
            return true;
        case TokenType::DIV_ASSIGN:
            return true;
        case TokenType::MOD_ASSIGN:
            return true;
        case TokenType::ADD_ASSIGN:
            return true;
        case TokenType::SUB_ASSIGN:
            return true;
        case TokenType::LEFT_ASSIGN:
            return true;
        case TokenType::RIGHT_ASSIGN:
            return true;
        case TokenType::OR_ASSIGN:
            return true;
        case TokenType::AND_ASSIGN:
            return true;
        case TokenType::XOR_ASSIGN:
            return true;
        default:
            return false;
        }
    }
    std::set<std::string> definedStruct;
    std::set<std::string> definedUnion;

private:
    void printBranches(const std::vector<bool> &isLast, std::ostream &os);
    void traverseTree(std::shared_ptr<Node> node, std::vector<bool> &isLast, std::ostream &os);
    TokenToString t;

public:
    AST(const std::string &path, Error &e);
    void printAST(std::ostream &os);
};
#endif