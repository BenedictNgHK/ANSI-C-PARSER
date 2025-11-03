#include "AST.hpp"

AST::AST(const std::string &path, Error &e) : Scanner(path, e), root(std::make_shared<Node>(Node(TokenType::TRANSLATION_UNIT)))
{
    root->t.lexeme = path;
    root = parsingFile(root);
}
std::shared_ptr<Node> AST::parsingFile(std::shared_ptr<Node> root)
{
    TokenToString t;

    while (true)
    {
        auto itr = peekNextToken();

        if (itr->type == TokenType::END)
            break;

        if (itr->type == TokenType::INCLUDE)
        {
            getNextToken(); // consume INCLUDE token
            root->children.push_back(includeStmt());
        }
        else
        {
            // Parse external declaration (function definition or declaration)
            auto extDecl = externalDeclaration();
            if (extDecl && !extDecl->children.empty())
                root->children.push_back(extDecl);
            else if (!extDecl)
                break; // End of file
        }
    }

    return root;
}
std::shared_ptr<Node> AST::includeStmt()
{
    auto itr = getNextToken();
    Node stmt(TokenType::INCLUDE_STMT);
    std::shared_ptr<Node> ret;
    ret = std::make_shared<Node>(std::move(stmt));

    auto tempType = itr->type;
    if (itr->type == TokenType::DOUBLE_QUOTE || itr->type == TokenType::LT)
        ret->children.push_back(std::make_shared<Node>(std::move(*itr)));

    else
    {
        loggedError.addError(lineNo, INCLUD_ERROR);
        return ret;
    }
    itr = getNextToken();

    if (itr->type == TokenType::INCLUDE_PATH)
        ret->children.push_back(std::make_shared<Node>(std::move(*itr)));
    else
    {
        loggedError.addError(lineNo, INCLUD_ERROR);
        return ret;
    }
    itr = getNextToken();

    if ((itr->type == TokenType::DOUBLE_QUOTE && tempType == TokenType::DOUBLE_QUOTE) || (itr->type == TokenType::GT && tempType == TokenType::LT))
        ret->children.push_back(std::make_shared<Node>(std::move(*itr)));
    else
    {
        loggedError.addError(lineNo, INCLUD_ERROR);
        return ret;
    }
    return ret;
}

std::shared_ptr<Node> AST::structUnionSpecifier(std::list<Token>::iterator begin)
{
    Node stmt(TokenType::STRUCT_UNION_SPECIFIER);
    std::shared_ptr<Node> ret;
    ret = std::make_shared<Node>(std::move(stmt));
    ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
    auto itr = getNextToken();
    if (itr->type == TokenType::ID)
    {
        ret->children.push_back(std::make_shared<Node>(std::move(*itr)));
        itr = peekNextToken();
        if (itr->type == TokenType::L_CUR)
        {
            itr = getNextToken();
            ret->children.push_back(std::make_shared<Node>(std::move(*itr)));
            itr = getNextToken();
            ret->children.push_back(std::make_shared<Node>(std::move(*structDeclarationList(itr))));
            itr = getNextToken();
            if (itr->type != TokenType::R_CUR)
            {
                loggedError.addError(lineNo, "Missing right curly bracket");
                ungetToken();
            }
            else
                ret->children.push_back(std::make_shared<Node>(std::move(*(itr))));
        }
    }
    else if (itr->type == TokenType::L_CUR)
    {
        ret->children.push_back(std::make_shared<Node>(std::move(*itr)));

        itr = getNextToken();
        ret->children.push_back(std::make_shared<Node>(std::move(*structDeclarationList(itr))));
        itr = getNextToken();
        if (itr->type != TokenType::R_CUR)
        {
            loggedError.addError(lineNo, "Missing right curly bracket");
            ungetToken();
        }
        else
            ret->children.push_back(std::make_shared<Node>(std::move(*(itr))));
    }
    else
        loggedError.addError(lineNo, STRUCT_UNION_ERROR);
    return ret;
}
std::shared_ptr<Node> AST::structDeclarationList(std::list<Token>::iterator begin)
{
    Node stmt(TokenType::STRUCT_DECLARATION_LIST);
    std::shared_ptr<Node> ret;
    ret = std::make_shared<Node>(std::move(stmt));
    while (1)
    {
        if (peekNextToken()->type == TokenType::R_CUR)
            break;
        begin = getNextToken();
        ret->children.push_back(structDeclaration(begin));
    }
    return ret;
}
std::shared_ptr<Node> AST::structDeclaration(std::list<Token>::iterator begin)
{
    Node stmt(TokenType::STRUCT_DECLARATION);
    std::shared_ptr<Node> ret;
    ret = std::make_shared<Node>(std::move(stmt));
    
    // Check for static assertion
    if (begin->type == TokenType::STATIC_ASSERT)
    {
        ret->children.push_back(staticAssertDeclaration(begin));
        return ret;
    }
    
    ret->children.push_back(specifierQualifierList(begin));
    begin = peekNextToken();
    
    // Check if we have declarators or if this is an anonymous struct/union
    if (begin->type != TokenType::SEMI_COLON)
    {
        begin = getNextToken();
        ret->children.push_back(structDeclaratorList(begin));
        begin = getNextToken();
    }
    else
    {
        begin = getNextToken();
    }
    
    if (begin->type == TokenType::SEMI_COLON)
        ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
    else
        loggedError.addGrammarError(lineNo, "Expected ';' after struct declaration");
    return ret;
}
std::shared_ptr<Node> AST::structDeclaratorList(std::list<Token>::iterator begin)
{
    Node stmt(TokenType::STRUCT_DECLARATION_LIST);
    std::shared_ptr<Node> ret;
    ret = std::make_shared<Node>(std::move(stmt));

    ret->children.push_back(structDeclarator(begin));

    while (peekNextToken()->type == TokenType::COMMA)
    {
        begin = getNextToken();
        ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
        begin = getNextToken();
        ret->children.push_back(structDeclarator(begin));
    }
    return ret;
}
std::shared_ptr<Node> AST::structDeclarator(std::list<Token>::iterator begin)
{
    Node stmt(TokenType::STRUCT_DECLARATOR);
    std::shared_ptr<Node> ret;
    ret = std::make_shared<Node>(std::move(stmt));
    if (begin->type != TokenType::COLON)
    {
        ret->children.push_back(declarator(begin));
        begin = peekNextToken();
        if (begin->type == TokenType::COLON)
        {
            begin = getNextToken();
            ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
            if (peekNextToken()->type == TokenType::CONSTANT)
            {
                begin = getNextToken();
                ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
            }
        }
    }
    else
    {
        if (begin->type == TokenType::COLON)
        {
            ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
            begin = peekNextToken();
            if (begin->type == TokenType::CONSTANT)
            {
                begin = getNextToken();
                ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
            }
        }
    }
    return ret;
}
std::shared_ptr<Node> AST::declarator(std::list<Token>::iterator begin)
{
    Node stmt(TokenType::DECLARATOR);
    std::shared_ptr<Node> ret;
    ret = std::make_shared<Node>(std::move(stmt));
    if (begin->type == TokenType::MUL)
    {
        ret->children.push_back(pointer(begin));
        begin = getNextToken();
    }
    ret->children.push_back(directDeclarator(begin));
    return ret;
}
std::shared_ptr<Node> AST::directDeclarator(std::list<Token>::iterator begin)
{
    Node stmt(TokenType::DIRECT_DECLARATOR);
    std::shared_ptr<Node> ret;
    ret = std::make_shared<Node>(std::move(stmt));
    if (begin->type == TokenType::ID || begin->type == TokenType::MAIN)
        ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
    else if (begin->type == TokenType::L_BR)
    {
        ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
        begin = getNextToken();
        ret->children.push_back(declarator(begin));
        begin = getNextToken();
        if (begin->type == TokenType::R_BR)
            ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
    }
    while (peekNextToken()->type == TokenType::L_SQR || peekNextToken()->type == TokenType::L_BR)
    {
        begin = getNextToken();
        ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
        auto next = getNextToken();
        if (begin->type == TokenType::L_SQR)
        {
            // Handle C11 array syntax: [ ], [ * ], [ STATIC ... ], [ type-qualifiers ... ]
            if (next->type == TokenType::R_SQR)
            {
                ret->children.push_back(std::make_shared<Node>(std::move(*next)));
            }
            else if (next->type == TokenType::MUL)
            {
                ret->children.push_back(std::make_shared<Node>(std::move(*next)));
                next = getNextToken();
                if (next->type == TokenType::R_SQR)
                    ret->children.push_back(std::make_shared<Node>(std::move(*next)));
            }
            else if (next->type == TokenType::STATIC)
            {
                ret->children.push_back(std::make_shared<Node>(std::move(*next)));
                next = getNextToken();
                
                // Check for optional type qualifier list after STATIC
                if (typeQualifier(next))
                {
                    ret->children.push_back(typeQualifierList(next));
                    next = getNextToken();
                }
                
                // Now expect assignment expression
                ret->children.push_back(assignmentExpression(next));
                next = getNextToken();
                if (next->type == TokenType::R_SQR)
                    ret->children.push_back(std::make_shared<Node>(std::move(*next)));
            }
            else if (typeQualifier(next))
            {
                ret->children.push_back(typeQualifierList(next));
                next = getNextToken();
                
                // After type qualifiers, can be: *, STATIC expr, expr, or ]
                if (next->type == TokenType::MUL)
                {
                    ret->children.push_back(std::make_shared<Node>(std::move(*next)));
                    next = getNextToken();
                }
                else if (next->type == TokenType::STATIC)
                {
                    ret->children.push_back(std::make_shared<Node>(std::move(*next)));
                    next = getNextToken();
                    ret->children.push_back(assignmentExpression(next));
                    next = getNextToken();
                }
                else if (next->type != TokenType::R_SQR)
                {
                    ret->children.push_back(assignmentExpression(next));
                    next = getNextToken();
                }
                
                if (next->type == TokenType::R_SQR)
                    ret->children.push_back(std::make_shared<Node>(std::move(*next)));
            }
            else
            {
                // Just an assignment expression
                ret->children.push_back(assignmentExpression(next));
                next = getNextToken();
                if (next->type == TokenType::R_SQR)
                    ret->children.push_back(std::make_shared<Node>(std::move(*next)));
            }
        }
        else if (begin->type == TokenType::L_BR)
        {
            if (next->type == TokenType::R_BR)
                ret->children.push_back(std::make_shared<Node>(std::move(*next)));

            else if (next->type == TokenType::ID)
                ret->children.push_back(identifierList(next));
            else
                ret->children.push_back(parameterTypeList(next));
            next = getNextToken();
            if (next->type == TokenType::R_BR)
                ret->children.push_back(std::make_shared<Node>(std::move(*next)));
        }
    }

    return ret;
}
std::shared_ptr<Node> AST::identifierList(std::list<Token>::iterator begin)
{
    Node stmt(TokenType::IDENTIFIER_LIST);
    std::shared_ptr<Node> ret;
    ret = std::make_shared<Node>(std::move(stmt));

    if (begin->type == TokenType::ID)
    {
        ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
        while (peekNextToken()->type == TokenType::COMMA)
        {
            begin = getNextToken();
            ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
            begin = peekNextToken();
            if (begin->type == TokenType::ID)
            {
                begin = getNextToken();
                ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
            }
        }
    }
    return ret;
}
std::shared_ptr<Node> AST::parameterTypeList(std::list<Token>::iterator begin)
{
    Node stmt(TokenType::PARAMETER_TYPE_LIST);
    std::shared_ptr<Node> ret;
    ret = std::make_shared<Node>(std::move(stmt));
    ret->children.push_back(parameterList(begin));
    if (peekNextToken()->type == TokenType::COMMA)
    {
        begin = getNextToken();
        ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
        if (peekNextToken()->type == TokenType::ELLIPSIS)
        {
            begin = getNextToken();
            ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
        }
    }
    return ret;
}
std::shared_ptr<Node> AST::parameterList(std::list<Token>::iterator begin)
{
    Node stmt(TokenType::PARAMETER_LIST);
    std::shared_ptr<Node> ret;
    ret = std::make_shared<Node>(std::move(stmt));
    ret->children.push_back(parameterDeclaration(begin));
    while (peekNextToken()->type == TokenType::COMMA)
    {
        begin = getNextToken();
        ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
        begin = getNextToken();
        ret->children.push_back(parameterDeclaration(begin));
    }
    return ret;
}
// Debugging
std::shared_ptr<Node> AST::parameterDeclaration(std::list<Token>::iterator begin)
{
    Node stmt(TokenType::PARAMETER_DECLARATION);
    std::shared_ptr<Node> ret;
    ret = std::make_shared<Node>(std::move(stmt));
    ret->children.push_back(declarationSpecifier(begin));
    begin = peekNextToken();
    if (begin->type == TokenType::MUL || begin->type == TokenType::ID || begin->type == TokenType::L_BR || begin->type == TokenType::L_SQR)
    {
        begin = getNextToken();
        if (begin->type == TokenType::ID)
            ret->children.push_back(declarator(begin));
        else if (begin->type == TokenType::L_SQR)
            ret->children.push_back(abstractDeclarator(begin));

        else
        {
            auto itr = begin;
            if (begin->type == TokenType::MUL)
            {
                auto type = itr->type;
                while (type == TokenType::MUL || type == TokenType::CONST || type == TokenType::VOLATILE)
                {
                    appendList(symbolTable);
                    itr = std::next(itr);
                    type = itr->type;
                }
                if ((type != TokenType::L_BR && type != TokenType::ID && type != TokenType::L_SQR) || type == TokenType::L_SQR)
                {
                    ret->children.push_back(abstractDeclarator(begin));
                    return ret;
                }
                if (type == TokenType::ID)
                {
                    ret->children.push_back(declarator(begin));
                    return ret;
                }
            }

            int bracketCount = 1;
            bool isDeclarator = false;
            for (; bracketCount != 0 && itr != symbolTable.end(); itr = std::next(itr))
            {
                appendList(symbolTable);
                if (itr->type == TokenType::L_BR)
                {
                    bracketCount++;
                    auto next = std::next(itr);

                    if (next->type == TokenType::ID && std::next(next)->type == TokenType::R_BR)
                    {

                        isDeclarator = true;
                        break;
                    }
                    if (std::next(itr)->type == TokenType::MUL && std::next(itr) != symbolTable.end())
                    {
                        auto type = std::next(itr)->type;
                        itr = std::next(itr);
                        while (type == TokenType::MUL || type == TokenType::CONST || type == TokenType::VOLATILE)
                        {
                            appendList(symbolTable);
                            itr = std::next(itr);
                            type = itr->type;
                        }
                        itr = std::prev(itr);
                    }
                }
                else if (itr->type == TokenType::R_BR)
                    bracketCount--;
            }
            if (isDeclarator)
                ret->children.push_back(declarator(begin));
            else
                ret->children.push_back(abstractDeclarator(begin));
        }
    }
    return ret;
}
std::shared_ptr<Node> AST::declarationSpecifier(std::list<Token>::iterator begin)
{
    Node stmt(TokenType::DECLARATION_SPECIFIERS);
    std::shared_ptr<Node> ret;
    ret = std::make_shared<Node>(std::move(stmt));
    auto start = begin;
    
    // First token must be a declaration specifier
    if (storageClassSpecifier(begin) || typeSpecifier(begin) || typeQualifier(begin) || 
        functionSpecifier(begin) || isAlignmentSpecifier(begin) ||
        (!begin->lexeme.empty() && isTypeName(begin->lexeme)))
    {
        // Handle first specifier
        if (structUnion(begin))
            ret->children.push_back(structUnionSpecifier(begin));
        else if (begin->type == TokenType::ENUM)
            ret->children.push_back(enumSpecifier(begin));
        else if (isAlignmentSpecifier(begin))
            ret->children.push_back(alignmentSpecifier(begin));
        else if (begin->type == TokenType::ATOMIC && peekNextToken()->type == TokenType::L_BR)
            ret->children.push_back(atomicTypeSpecifier(begin));
        else
            ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
        
        // Parse additional specifiers
        while (true)
        {
            begin = peekNextToken();
            if (storageClassSpecifier(begin) || typeSpecifier(begin) || typeQualifier(begin) || 
                functionSpecifier(begin) || isAlignmentSpecifier(begin) ||
                (!begin->lexeme.empty() && isTypeName(begin->lexeme)))
            {
                begin = getNextToken();
                if (structUnion(begin))
                    ret->children.push_back(structUnionSpecifier(begin));
                else if (begin->type == TokenType::ENUM)
                    ret->children.push_back(enumSpecifier(begin));
                else if (isAlignmentSpecifier(begin))
                    ret->children.push_back(alignmentSpecifier(begin));
                else if (begin->type == TokenType::ATOMIC && peekNextToken()->type == TokenType::L_BR)
                    ret->children.push_back(atomicTypeSpecifier(begin));
                else
                    ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
            }
            else
                break;
        }
        
        // Handle typedef name tracking
        if (start->type == TokenType::TYPEDEF)
        {
            auto itr = currentToken;
            while (itr->type != TokenType::ID && itr != symbolTable.end())
            {
                appendList(symbolTable);
                itr = std::next(itr);
            }
            if (itr != symbolTable.end() && itr->type == TokenType::ID)
                definedTypeNames.insert(itr->lexeme);
        }
    }
    return ret;
}
std::shared_ptr<Node> AST::abstractDeclarator(std::list<Token>::iterator begin)
{
    Node stmt(TokenType::ABSTRACT_DECLARATOR);
    std::shared_ptr<Node> ret;
    ret = std::make_shared<Node>(std::move(stmt));

    if (begin->type == TokenType::MUL)
    {
        ret->children.push_back(pointer(begin));
        auto peeked = peekNextToken();
        if (peeked->type == TokenType::L_BR || peeked->type == TokenType::L_SQR)
        {
            begin = getNextToken();
            ret->children.push_back(directAbstractDeclarator(begin));
        }
    }
    else if (begin->type == TokenType::L_SQR || begin->type == TokenType::L_BR)
        ret->children.push_back(directAbstractDeclarator(begin));
    return ret;
}
std::shared_ptr<Node> AST::initDeclarator(std::list<Token>::iterator begin)
{
    Node stmt(TokenType::INIT_DECLARATOR);
    std::shared_ptr<Node> ret;
    ret = std::make_shared<Node>(std::move(stmt));
    ret->children.push_back(declarator(begin));
    if (peekNextToken()->type == TokenType::ASSIGN)
    {
        begin = getNextToken();
        ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
        begin = getNextToken();
        ret->children.push_back(initializer(begin));
    }
    return ret;
}
std::shared_ptr<Node> AST::initializer(std::list<Token>::iterator begin)
{
    Node stmt(TokenType::INITIALIZER);
    std::shared_ptr<Node> ret = std::make_shared<Node>(std::move(stmt));

    if (begin->type == TokenType::L_CUR)
    {
        ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
        begin = getNextToken();
        ret->children.push_back(initializerList(begin));
        begin = getNextToken();
        if (begin->type == TokenType::COMMA)
        {
            ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
            begin = getNextToken();
        }
        if (begin->type == TokenType::R_CUR)
            ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
        else
        {
            loggedError.addError(lineNo, "Expected '}' in initializer");
            ungetToken();
        }
    }
    else
    {
        ret->children.push_back(assignmentExpression(begin));
    }
    return ret;
}

std::shared_ptr<Node> AST::initializerList(std::list<Token>::iterator begin)
{
    Node stmt(TokenType::INITIALIZER_LIST);
    std::shared_ptr<Node> ret = std::make_shared<Node>(std::move(stmt));

    // Check for designation
    if ((begin->type == TokenType::L_SQR) || (begin->type == TokenType::DOT))
    {
        ret->children.push_back(designation(begin));
        begin = getNextToken();
    }
    ret->children.push_back(initializer(begin));

    while (peekNextToken()->type == TokenType::COMMA && peekNextToken() != symbolTable.end())
    {
        auto next = peekNextToken();
        if (std::next(next) != symbolTable.end() && std::next(next)->type == TokenType::R_CUR)
            break;

        begin = getNextToken();
        ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
        begin = getNextToken();

        // Check for designation
        if ((begin->type == TokenType::L_SQR) || (begin->type == TokenType::DOT))
        {
            ret->children.push_back(designation(begin));
            begin = getNextToken();
        }
        ret->children.push_back(initializer(begin));
    }
    return ret;
}

std::shared_ptr<Node> AST::designation(std::list<Token>::iterator begin)
{
    Node stmt(TokenType::DESIGNATION);
    std::shared_ptr<Node> ret = std::make_shared<Node>(std::move(stmt));
    ret->children.push_back(designatorList(begin));
    begin = getNextToken();
    if (begin->type == TokenType::ASSIGN)
        ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
    else
    {
        loggedError.addError(lineNo, "Expected '=' after designator list");
        ungetToken();
    }
    return ret;
}

std::shared_ptr<Node> AST::designatorList(std::list<Token>::iterator begin)
{
    Node stmt(TokenType::DESIGNATOR_LIST);
    std::shared_ptr<Node> ret = std::make_shared<Node>(std::move(stmt));
    ret->children.push_back(designator(begin));

    while (peekNextToken()->type == TokenType::L_SQR || peekNextToken()->type == TokenType::DOT)
    {
        begin = getNextToken();
        ret->children.push_back(designator(begin));
    }
    return ret;
}

std::shared_ptr<Node> AST::designator(std::list<Token>::iterator begin)
{
    Node stmt(TokenType::DESIGNATOR);
    std::shared_ptr<Node> ret = std::make_shared<Node>(std::move(stmt));

    if (begin->type == TokenType::L_SQR)
    {
        ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
        begin = getNextToken();
        ret->children.push_back(constantExpression(begin));
        begin = getNextToken();
        if (begin->type == TokenType::R_SQR)
            ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
    }
    else if (begin->type == TokenType::DOT)
    {
        ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
        begin = getNextToken();
        if (begin->type == TokenType::ID)
            ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
        else
            loggedError.addError(lineNo, "Expected identifier after '.'");
    }
    return ret;
}

std::shared_ptr<Node> AST::directAbstractDeclarator(std::list<Token>::iterator begin)
{
    Node stmt(TokenType::DIRECT_ABSTRACT_DECLARATOR);
    std::shared_ptr<Node> ret;
    ret = std::make_shared<Node>(std::move(stmt));
    ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
    if (begin->type == TokenType::L_BR)
    {

        begin = getNextToken();
        if (begin->type == TokenType::R_BR)
        {
            ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
        }

        else if (storageClassSpecifier(begin) || typeSpecifier(begin))
        {
            ret->children.push_back(parameterTypeList(begin));
            begin = getNextToken();

            if (begin->type == TokenType::R_BR)
                ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
        }
        else
        {
            ret->children.push_back(abstractDeclarator(begin));
            begin = getNextToken();

            if (begin->type == TokenType::R_BR)
                ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
        }
    }
    else if (begin->type == TokenType::L_SQR)
    {
        begin = getNextToken();
        
        // Handle C11 array syntax
        if (begin->type == TokenType::R_SQR)
        {
            ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
        }
        else if (begin->type == TokenType::MUL)
        {
            ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
            begin = getNextToken();
            if (begin->type == TokenType::R_SQR)
                ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
        }
        else if (begin->type == TokenType::STATIC)
        {
            ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
            begin = getNextToken();
            
            // Optional type qualifier list
            if (typeQualifier(begin))
            {
                ret->children.push_back(typeQualifierList(begin));
                begin = getNextToken();
            }
            
            // Assignment expression
            ret->children.push_back(assignmentExpression(begin));
            begin = getNextToken();
            if (begin->type == TokenType::R_SQR)
                ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
        }
        else if (typeQualifier(begin))
        {
            ret->children.push_back(typeQualifierList(begin));
            begin = getNextToken();
            
            if (begin->type == TokenType::MUL)
            {
                ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
                begin = getNextToken();
            }
            else if (begin->type == TokenType::STATIC)
            {
                ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
                begin = getNextToken();
                ret->children.push_back(assignmentExpression(begin));
                begin = getNextToken();
            }
            else if (begin->type != TokenType::R_SQR)
            {
                ret->children.push_back(assignmentExpression(begin));
                begin = getNextToken();
            }
            
            if (begin->type == TokenType::R_SQR)
                ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
        }
        else
        {
            // Just assignment expression
            ret->children.push_back(assignmentExpression(begin));
            begin = getNextToken();
            if (begin->type == TokenType::R_SQR)
                ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
        }
    }
    while (1)
    {
        auto peeked = peekNextToken();
        if (peeked->type == TokenType::L_BR)
        {

            begin = getNextToken();
            ret->children.push_back(std::make_shared<Node>(std::move(*begin)));

            begin = getNextToken();
            if (begin->type == TokenType::R_BR)
                ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
            else if (storageClassSpecifier(begin) || typeSpecifier(begin))
            {
                ret->children.push_back(parameterTypeList(begin));
                begin = getNextToken();

                if (begin->type == TokenType::R_BR)
                    ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
            }
        }
        else if (peeked->type == TokenType::L_SQR)
        {
            begin = getNextToken();
            ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
            begin = getNextToken();

            // Handle C11 array syntax
            if (begin->type == TokenType::R_SQR)
            {
                ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
            }
            else if (begin->type == TokenType::MUL)
            {
                ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
                begin = getNextToken();
                if (begin->type == TokenType::R_SQR)
                    ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
            }
            else if (begin->type == TokenType::STATIC)
            {
                ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
                begin = getNextToken();
                
                if (typeQualifier(begin))
                {
                    ret->children.push_back(typeQualifierList(begin));
                    begin = getNextToken();
                }
                
                ret->children.push_back(assignmentExpression(begin));
                begin = getNextToken();
                if (begin->type == TokenType::R_SQR)
                    ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
            }
            else if (typeQualifier(begin))
            {
                ret->children.push_back(typeQualifierList(begin));
                begin = getNextToken();
                
                if (begin->type == TokenType::MUL)
                {
                    ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
                    begin = getNextToken();
                }
                else if (begin->type == TokenType::STATIC)
                {
                    ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
                    begin = getNextToken();
                    ret->children.push_back(assignmentExpression(begin));
                    begin = getNextToken();
                }
                else if (begin->type != TokenType::R_SQR)
                {
                    ret->children.push_back(assignmentExpression(begin));
                    begin = getNextToken();
                }
                
                if (begin->type == TokenType::R_SQR)
                    ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
            }
            else
            {
                ret->children.push_back(assignmentExpression(begin));
                begin = getNextToken();
                if (begin->type == TokenType::R_SQR)
                    ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
            }
        }
        else
            break;
    }

    return ret;
}
std::shared_ptr<Node> AST::pointer(std::list<Token>::iterator begin)
{
    Node stmt(TokenType::POINTER);
    std::shared_ptr<Node> ret;
    ret = std::make_shared<Node>(std::move(stmt));
    if (begin->type == TokenType::MUL)
    {
        ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
        begin = peekNextToken();
        if (begin->type == TokenType::MUL)
        {
            begin = getNextToken();
            ret->children.push_back(pointer(begin));
        }
        else if (typeQualifier(begin))
        {
            begin = getNextToken();
            ret->children.push_back(typeQualifierList(begin));
            if (peekNextToken()->type == TokenType::MUL)
            {
                begin = getNextToken();
                ret->children.push_back(pointer(begin));
            }
        }
    }

    return ret;
}
std::shared_ptr<Node> AST::typeQualifierList(std::list<Token>::iterator begin)
{
    Node stmt(TokenType::TYPE_QUALIFIER_LIST);
    std::shared_ptr<Node> ret;
    ret = std::make_shared<Node>(std::move(stmt));
    while (typeQualifier(begin))
    {
        ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
        begin = getNextToken();
    }
    ungetToken();
    return ret;
}
std::shared_ptr<Node> AST::specifierQualifierList(std::list<Token>::iterator begin)
{
    Node stmt(TokenType::SPECIFIER_QUALIFIER_LIST);
    std::shared_ptr<Node> ret;
    ret = std::make_shared<Node>(std::move(stmt));
    while (typeQualifier(begin) || typeSpecifier(begin))
    {
        if (structUnion(begin))
            ret->children.push_back((structUnionSpecifier(begin)));
        else if (begin->type == TokenType::ENUM)
        {
            ret->children.push_back(enumSpecifier(begin));
        }
        else
            ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
        begin = getNextToken();
    }
    ungetToken();
    return ret;
}
std::shared_ptr<Node> AST::enumSpecifier(std::list<Token>::iterator begin)
{
    Node stmt(TokenType::ENUM_SPECIFIER);
    std::shared_ptr<Node> ret;
    ret = std::make_shared<Node>(std::move(stmt));
    if (begin->type == TokenType::ENUM)
        ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
    begin = getNextToken();
    if (begin->type == TokenType::L_CUR)
    {
        ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
        begin = getNextToken();
        ret->children.push_back(enumeratorList(begin));
        begin = getNextToken();
        if (begin->type == TokenType::R_CUR)
            ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
    }
    if (begin->type == TokenType::ID)
    {
        ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
        begin = peekNextToken();
        if (begin->type == TokenType::L_CUR)
        {
            begin = getNextToken();
            ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
            begin = getNextToken();
            ret->children.push_back(enumeratorList(begin));
            begin = getNextToken();
            if (begin->type == TokenType::R_CUR)
                ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
        }
    }
    return ret;
}
std::shared_ptr<Node> AST::enumerator(std::list<Token>::iterator begin)
{
    Node stmt(TokenType::ENUMERATOR);
    std::shared_ptr<Node> ret;
    ret = std::make_shared<Node>(std::move(stmt));
    if (begin->type == TokenType::ID)
    {
        ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
        begin = peekNextToken();
        if (begin->type == TokenType::ASSIGN)
        {
            begin = getNextToken();
            ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
            begin = peekNextToken();
            if (begin->type == TokenType::CONSTANT)
            {
                begin = getNextToken();
                ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
            }
        }
    }
    return ret;
}
std::shared_ptr<Node> AST::enumeratorList(std::list<Token>::iterator begin)
{
    Node stmt(TokenType::ENUMERATOR_LIST);
    std::shared_ptr<Node> ret;
    ret = std::make_shared<Node>(std::move(stmt));
    if (begin->type == TokenType::ID)
    {
        ret->children.push_back(enumerator(begin));
        while (peekNextToken()->type == TokenType::COMMA)
        {
            begin = getNextToken();
            ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
            begin = peekNextToken();
            if (begin->type == TokenType::ID)
            {
                begin = getNextToken();
                ret->children.push_back(enumerator(begin));
            }
        }
    }
    return ret;
}
// Declaration and function definition functions
std::shared_ptr<Node> AST::declaration(std::list<Token>::iterator begin)
{
    Node stmt(TokenType::DECLARATION);
    std::shared_ptr<Node> ret = std::make_shared<Node>(std::move(stmt));

    // Check for static assertion
    if (begin->type == TokenType::STATIC_ASSERT)
    {
        ret->children.push_back(staticAssertDeclaration(begin));
        return ret;
    }

    ret->children.push_back(declarationSpecifier(begin));

    auto next = peekNextToken();
    if (next->type != TokenType::SEMI_COLON)
    {
        begin = getNextToken();
        ret->children.push_back(initDeclaratorList(begin));
        begin = getNextToken();
    }
    else
    {
        begin = getNextToken();
    }

    if (begin->type == TokenType::SEMI_COLON)
        ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
    else
        loggedError.addGrammarError(lineNo, "Expected ';' after declaration");

    return ret;
}

std::shared_ptr<Node> AST::initDeclaratorList(std::list<Token>::iterator begin)
{
    Node stmt(TokenType::INIT_DECLARATOR_LIST);
    std::shared_ptr<Node> ret = std::make_shared<Node>(std::move(stmt));

    ret->children.push_back(initDeclarator(begin));

    while (peekNextToken()->type == TokenType::COMMA)
    {
        begin = getNextToken();
        ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
        begin = getNextToken();
        ret->children.push_back(initDeclarator(begin));
    }
    return ret;
}

std::shared_ptr<Node> AST::functionDefinition(std::list<Token>::iterator begin)
{
    Node stmt(TokenType::FUNCTION_DEFINITION);
    std::shared_ptr<Node> ret = std::make_shared<Node>(std::move(stmt));

    ret->children.push_back(declarationSpecifier(begin));
    begin = getNextToken();
    ret->children.push_back(declarator(begin));

    // Check if there's a declaration list (old-style K&R C)
    auto next = peekNextToken();
    if (storageClassSpecifier(next) || typeSpecifier(next) || typeQualifier(next))
    {
        begin = getNextToken();
        ret->children.push_back(declarationList(begin));
        begin = getNextToken();
    }
    else
    {
        begin = getNextToken();
    }

    ret->children.push_back(compoundStatement(begin));
    return ret;
}

std::shared_ptr<Node> AST::declarationList(std::list<Token>::iterator begin)
{
    Node stmt(TokenType::DECLARATION_LIST);
    std::shared_ptr<Node> ret = std::make_shared<Node>(std::move(stmt));

    ret->children.push_back(declaration(begin));

    while (true)
    {
        auto next = peekNextToken();
        if (storageClassSpecifier(next) || typeSpecifier(next) || typeQualifier(next))
        {
            begin = getNextToken();
            ret->children.push_back(declaration(begin));
        }
        else
            break;
    }
    return ret;
}

std::shared_ptr<Node> AST::externalDeclaration()
{
    Node stmt(TokenType::EXTERNAL_DECLARATION);
    std::shared_ptr<Node> ret = std::make_shared<Node>(std::move(stmt));

    auto begin = getNextToken();

    if (begin->type == TokenType::END)
        return nullptr;

    // Check if this is a function definition or declaration
    // We need to look ahead to distinguish between them
    auto saved = currentToken;

    // Parse declaration specifiers (including C11 specifiers)
    if (!storageClassSpecifier(begin) && !typeSpecifier(begin) && !typeQualifier(begin) &&
        !functionSpecifier(begin) && !isAlignmentSpecifier(begin) &&
        !(begin->type == TokenType::STATIC_ASSERT) &&
        !(!begin->lexeme.empty() && isTypeName(begin->lexeme)))
    {
        loggedError.addGrammarError(lineNo, "Expected declaration or function definition");
        return ret;
    }

    // Skip past declaration specifiers to find declarator
    while (storageClassSpecifier(peekNextToken()) || typeSpecifier(peekNextToken()) ||
           typeQualifier(peekNextToken()) || functionSpecifier(peekNextToken()) ||
           isAlignmentSpecifier(peekNextToken()))
    {
        getNextToken();
    }

    // Skip past declarator to check what follows
    int bracketDepth = 0;
    bool foundCompound = false;
    while (true)
    {
        auto tok = peekNextToken();
        if (tok->type == TokenType::L_CUR && bracketDepth == 0)
        {
            foundCompound = true;
            break;
        }
        if (tok->type == TokenType::SEMI_COLON)
            break;
        if (tok->type == TokenType::END)
            break;
        if (tok->type == TokenType::L_BR)
            bracketDepth++;
        if (tok->type == TokenType::R_BR)
        {
            bracketDepth--;
            if (bracketDepth < 0)
                break;
        }
        getNextToken();
    }

    // Restore position
    currentToken = saved;
    begin = currentToken;

    if (foundCompound)
        ret->children.push_back(functionDefinition(begin));
    else
        ret->children.push_back(declaration(begin));

    return ret;
}

// Statement parsing functions
std::shared_ptr<Node> AST::statement(std::list<Token>::iterator begin)
{
    Node stmt(TokenType::STATEMENT);
    std::shared_ptr<Node> ret = std::make_shared<Node>(std::move(stmt));

    if (begin->type == TokenType::CASE || begin->type == TokenType::DEFAULT ||
        (begin->type == TokenType::ID && peekNextToken()->type == TokenType::COLON))
    {
        ret->children.push_back(labeledStatement(begin));
    }
    else if (begin->type == TokenType::L_CUR)
    {
        ret->children.push_back(compoundStatement(begin));
    }
    else if (begin->type == TokenType::IF || begin->type == TokenType::SWITCH)
    {
        ret->children.push_back(selectionStatement(begin));
    }
    else if (begin->type == TokenType::WHILE || begin->type == TokenType::DO || begin->type == TokenType::FOR)
    {
        ret->children.push_back(iterationStatement(begin));
    }
    else if (begin->type == TokenType::GOTO || begin->type == TokenType::CONT ||
             begin->type == TokenType::BRK || begin->type == TokenType::RETURN)
    {
        ret->children.push_back(jumpStatement(begin));
    }
    else
    {
        ret->children.push_back(expressionStatement(begin));
    }
    return ret;
}

std::shared_ptr<Node> AST::labeledStatement(std::list<Token>::iterator begin)
{
    Node stmt(TokenType::LABELED_STATEMENT);
    std::shared_ptr<Node> ret = std::make_shared<Node>(std::move(stmt));

    if (begin->type == TokenType::ID)
    {
        ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
        begin = getNextToken();
        if (begin->type == TokenType::COLON)
        {
            ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
            begin = getNextToken();
            ret->children.push_back(statement(begin));
        }
    }
    else if (begin->type == TokenType::CASE)
    {
        ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
        begin = getNextToken();
        ret->children.push_back(constantExpression(begin));
        begin = getNextToken();
        if (begin->type == TokenType::COLON)
        {
            ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
            begin = getNextToken();
            ret->children.push_back(statement(begin));
        }
    }
    else if (begin->type == TokenType::DEFAULT)
    {
        ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
        begin = getNextToken();
        if (begin->type == TokenType::COLON)
        {
            ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
            begin = getNextToken();
            ret->children.push_back(statement(begin));
        }
    }
    return ret;
}

std::shared_ptr<Node> AST::compoundStatement(std::list<Token>::iterator begin)
{
    Node stmt(TokenType::COMPOUND_STATEMENT);
    std::shared_ptr<Node> ret = std::make_shared<Node>(std::move(stmt));

    if (begin->type == TokenType::L_CUR)
    {
        ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
        begin = peekNextToken();
        if (begin->type == TokenType::R_CUR)
        {
            begin = getNextToken();
            ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
        }
        else
        {
            begin = getNextToken();
            ret->children.push_back(blockItemList(begin));
            begin = getNextToken();
            if (begin->type == TokenType::R_CUR)
                ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
            else
                loggedError.addError(lineNo, "Expected '}' in compound statement");
        }
    }
    return ret;
}

std::shared_ptr<Node> AST::blockItemList(std::list<Token>::iterator begin)
{
    Node stmt(TokenType::BLOCK_ITEM_LIST);
    std::shared_ptr<Node> ret = std::make_shared<Node>(std::move(stmt));

    ret->children.push_back(blockItem(begin));

    while (peekNextToken()->type != TokenType::R_CUR && peekNextToken()->type != TokenType::END)
    {
        begin = getNextToken();
        ret->children.push_back(blockItem(begin));
    }
    return ret;
}

std::shared_ptr<Node> AST::blockItem(std::list<Token>::iterator begin)
{
    Node stmt(TokenType::BLOCK_ITEM);
    std::shared_ptr<Node> ret = std::make_shared<Node>(std::move(stmt));

    if (storageClassSpecifier(begin) || typeSpecifier(begin) || typeQualifier(begin) || 
        functionSpecifier(begin) || isAlignmentSpecifier(begin) || begin->type == TokenType::STATIC_ASSERT ||
        (!begin->lexeme.empty() && isTypeName(begin->lexeme)))
    {
        ret->children.push_back(declaration(begin));
    }
    else
    {
        ret->children.push_back(statement(begin));
    }
    return ret;
}

std::shared_ptr<Node> AST::expressionStatement(std::list<Token>::iterator begin)
{
    Node stmt(TokenType::EXPRESSION_STATEMENT);
    std::shared_ptr<Node> ret = std::make_shared<Node>(std::move(stmt));

    if (begin->type == TokenType::SEMI_COLON)
    {
        ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
    }
    else
    {
        ret->children.push_back(expression(begin));
        begin = getNextToken();
        if (begin->type == TokenType::SEMI_COLON)
            ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
        else
            loggedError.addError(lineNo, "Expected ';' after expression");
    }
    return ret;
}

std::shared_ptr<Node> AST::selectionStatement(std::list<Token>::iterator begin)
{
    Node stmt(TokenType::SELECTION_STATEMENT);
    std::shared_ptr<Node> ret = std::make_shared<Node>(std::move(stmt));

    if (begin->type == TokenType::IF)
    {
        ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
        begin = getNextToken();
        if (begin->type == TokenType::L_BR)
        {
            ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
            begin = getNextToken();
            ret->children.push_back(expression(begin));
            begin = getNextToken();
            if (begin->type == TokenType::R_BR)
            {
                ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
                begin = getNextToken();
                ret->children.push_back(statement(begin));

                if (peekNextToken()->type == TokenType::ELSE)
                {
                    begin = getNextToken();
                    ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
                    begin = getNextToken();
                    ret->children.push_back(statement(begin));
                }
            }
        }
    }
    else if (begin->type == TokenType::SWITCH)
    {
        ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
        begin = getNextToken();
        if (begin->type == TokenType::L_BR)
        {
            ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
            begin = getNextToken();
            ret->children.push_back(expression(begin));
            begin = getNextToken();
            if (begin->type == TokenType::R_BR)
            {
                ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
                begin = getNextToken();
                ret->children.push_back(statement(begin));
            }
        }
    }
    return ret;
}

std::shared_ptr<Node> AST::iterationStatement(std::list<Token>::iterator begin)
{
    Node stmt(TokenType::ITERATION_STATEMENT);
    std::shared_ptr<Node> ret = std::make_shared<Node>(std::move(stmt));

    if (begin->type == TokenType::WHILE)
    {
        ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
        begin = getNextToken();
        if (begin->type == TokenType::L_BR)
        {
            ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
            begin = getNextToken();
            ret->children.push_back(expression(begin));
            begin = getNextToken();
            if (begin->type == TokenType::R_BR)
            {
                ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
                begin = getNextToken();
                ret->children.push_back(statement(begin));
            }
        }
    }
    else if (begin->type == TokenType::DO)
    {
        ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
        begin = getNextToken();
        ret->children.push_back(statement(begin));
        begin = getNextToken();
        if (begin->type == TokenType::WHILE)
        {
            ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
            begin = getNextToken();
            if (begin->type == TokenType::L_BR)
            {
                ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
                begin = getNextToken();
                ret->children.push_back(expression(begin));
                begin = getNextToken();
                if (begin->type == TokenType::R_BR)
                {
                    ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
                    begin = getNextToken();
                    if (begin->type == TokenType::SEMI_COLON)
                        ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
                }
            }
        }
    }
    else if (begin->type == TokenType::FOR)
    {
        ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
        begin = getNextToken();
        if (begin->type == TokenType::L_BR)
        {
            ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
            begin = getNextToken();

            // First part: declaration or expression-statement
            if (storageClassSpecifier(begin) || typeSpecifier(begin) || typeQualifier(begin))
            {
                ret->children.push_back(declaration(begin));
            }
            else
            {
                ret->children.push_back(expressionStatement(begin));
            }

            begin = getNextToken();
            ret->children.push_back(expressionStatement(begin));

            begin = peekNextToken();
            if (begin->type != TokenType::R_BR)
            {
                begin = getNextToken();
                ret->children.push_back(expression(begin));
                begin = getNextToken();
            }
            else
            {
                begin = getNextToken();
            }

            if (begin->type == TokenType::R_BR)
            {
                ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
                begin = getNextToken();
                ret->children.push_back(statement(begin));
            }
        }
    }
    return ret;
}

std::shared_ptr<Node> AST::jumpStatement(std::list<Token>::iterator begin)
{
    Node stmt(TokenType::JUMP_STATEMENT);
    std::shared_ptr<Node> ret = std::make_shared<Node>(std::move(stmt));

    if (begin->type == TokenType::GOTO)
    {
        ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
        begin = getNextToken();
        if (begin->type == TokenType::ID)
        {
            ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
            begin = getNextToken();
            if (begin->type == TokenType::SEMI_COLON)
                ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
        }
    }
    else if (begin->type == TokenType::CONT || begin->type == TokenType::BRK)
    {
        ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
        begin = getNextToken();
        if (begin->type == TokenType::SEMI_COLON)
            ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
    }
    else if (begin->type == TokenType::RETURN)
    {
        ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
        begin = peekNextToken();
        if (begin->type != TokenType::SEMI_COLON)
        {
            begin = getNextToken();
            ret->children.push_back(expression(begin));
            begin = getNextToken();
        }
        else
        {
            begin = getNextToken();
        }
        if (begin->type == TokenType::SEMI_COLON)
            ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
    }
    return ret;
}

// Expression parsing functions
std::shared_ptr<Node> AST::primaryExpression(std::list<Token>::iterator begin)
{
    Node stmt(TokenType::PRIMARY_EXPRESSION);
    std::shared_ptr<Node> ret = std::make_shared<Node>(std::move(stmt));

    if (begin->type == TokenType::ID || begin->type == TokenType::CONSTANT || 
        begin->type == TokenType::STRING_LITERAL || begin->type == TokenType::FUNC_NAME)
    {
        ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
    }
    else if (begin->type == TokenType::GENERIC)
    {
        ret->children.push_back(genericSelection(begin));
    }
    else if (begin->type == TokenType::L_BR)
    {
        ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
        begin = getNextToken();
        ret->children.push_back(expression(begin));
        begin = getNextToken();
        if (begin->type == TokenType::R_BR)
            ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
        else
            loggedError.addGrammarError(lineNo, "Expected ')' in primary expression");
    }
    return ret;
}

std::shared_ptr<Node> AST::postfixExpression(std::list<Token>::iterator begin)
{
    Node stmt(TokenType::POSTFIX_EXPRESSION);
    std::shared_ptr<Node> ret = std::make_shared<Node>(std::move(stmt));
    ret->children.push_back(primaryExpression(begin));

    while (true)
    {
        auto next = peekNextToken();
        if (next->type == TokenType::L_SQR)
        {
            begin = getNextToken();
            ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
            begin = getNextToken();
            ret->children.push_back(expression(begin));
            begin = getNextToken();
            if (begin->type == TokenType::R_SQR)
                ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
        }
        else if (next->type == TokenType::L_BR)
        {
            begin = getNextToken();
            ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
            begin = getNextToken();
            if (begin->type == TokenType::R_BR)
                ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
            else
            {
                ret->children.push_back(argumentExpressionList(begin));
                begin = getNextToken();
                if (begin->type == TokenType::R_BR)
                    ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
            }
        }
        else if (next->type == TokenType::DOT || next->type == TokenType::ARRORW)
        {
            begin = getNextToken();
            ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
            begin = getNextToken();
            if (begin->type == TokenType::ID)
                ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
        }
        else if (next->type == TokenType::INC || next->type == TokenType::DEC)
        {
            begin = getNextToken();
            ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
        }
        else
            break;
    }
    return ret;
}

std::shared_ptr<Node> AST::argumentExpressionList(std::list<Token>::iterator begin)
{
    Node stmt(TokenType::ARGUMENT_EXPRESSION_LIST);
    std::shared_ptr<Node> ret = std::make_shared<Node>(std::move(stmt));
    ret->children.push_back(assignmentExpression(begin));

    while (peekNextToken()->type == TokenType::COMMA)
    {
        begin = getNextToken();
        ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
        begin = getNextToken();
        ret->children.push_back(assignmentExpression(begin));
    }
    return ret;
}

std::shared_ptr<Node> AST::unaryExpression(std::list<Token>::iterator begin)
{
    Node stmt(TokenType::UNARY_EXPRESSION);
    std::shared_ptr<Node> ret = std::make_shared<Node>(std::move(stmt));

    if (begin->type == TokenType::INC || begin->type == TokenType::DEC)
    {
        ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
        begin = getNextToken();
        ret->children.push_back(unaryExpression(begin));
    }
    else if (begin->type == TokenType::SIZEOF || begin->type == TokenType::ALIGNOF)
    {
        ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
        begin = getNextToken();
        if (begin->type == TokenType::L_BR)
        {
            ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
            begin = getNextToken();
            ret->children.push_back(typeName(begin));
            begin = getNextToken();
            if (begin->type == TokenType::R_BR)
                ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
            else
                loggedError.addGrammarError(lineNo, "Expected ')' after type name");
        }
        else
        {
            // sizeof can have unary expression, but alignof requires parentheses
            if (begin->type == TokenType::ALIGNOF)
                loggedError.addGrammarError(lineNo, "_Alignof requires parentheses");
            else
                ret->children.push_back(unaryExpression(begin));
        }
    }
    else if (begin->type == TokenType::REFERENCE || begin->type == TokenType::MUL ||
             begin->type == TokenType::PLUS || begin->type == TokenType::MINUS ||
             begin->type == TokenType::TILDE || begin->type == TokenType::NOT)
    {
        ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
        begin = getNextToken();
        ret->children.push_back(castExpression(begin));
    }
    else
    {
        ret->children.push_back(postfixExpression(begin));
    }
    return ret;
}

std::shared_ptr<Node> AST::castExpression(std::list<Token>::iterator begin)
{
    Node stmt(TokenType::CAST_EXPRESSION);
    std::shared_ptr<Node> ret = std::make_shared<Node>(std::move(stmt));

    // Check if this is a cast: '(' type-name ')'
    if (begin->type == TokenType::L_BR)
    {
        auto saved = currentToken;
        begin = getNextToken();
        if (typeSpecifier(begin) || typeQualifier(begin))
        {
            currentToken = std::prev(currentToken);
            begin = getNextToken();
            ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
            begin = getNextToken();
            ret->children.push_back(typeName(begin));
            begin = getNextToken();
            if (begin->type == TokenType::R_BR)
            {
                ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
                begin = getNextToken();
                ret->children.push_back(castExpression(begin));
                return ret;
            }
        }
        currentToken = saved;
        begin = currentToken;
    }
    ret->children.push_back(unaryExpression(begin));
    return ret;
}

std::shared_ptr<Node> AST::multiplicativeExpression(std::list<Token>::iterator begin)
{
    Node stmt(TokenType::MULTIPLICATIVE_EXPRESSION);
    std::shared_ptr<Node> ret = std::make_shared<Node>(std::move(stmt));
    ret->children.push_back(castExpression(begin));

    while (true)
    {
        auto next = peekNextToken();
        if (next->type == TokenType::MUL || next->type == TokenType::DIV || next->type == TokenType::MOD)
        {
            begin = getNextToken();
            ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
            begin = getNextToken();
            ret->children.push_back(castExpression(begin));
        }
        else
            break;
    }
    return ret;
}

std::shared_ptr<Node> AST::additiveExpression(std::list<Token>::iterator begin)
{
    Node stmt(TokenType::ADDITIVE_EXPRESSION);
    std::shared_ptr<Node> ret = std::make_shared<Node>(std::move(stmt));
    ret->children.push_back(multiplicativeExpression(begin));

    while (true)
    {
        auto next = peekNextToken();
        if (next->type == TokenType::PLUS || next->type == TokenType::MINUS)
        {
            begin = getNextToken();
            ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
            begin = getNextToken();
            ret->children.push_back(multiplicativeExpression(begin));
        }
        else
            break;
    }
    return ret;
}

std::shared_ptr<Node> AST::shiftExpression(std::list<Token>::iterator begin)
{
    Node stmt(TokenType::SHIFT_EXPRESSION);
    std::shared_ptr<Node> ret = std::make_shared<Node>(std::move(stmt));
    ret->children.push_back(additiveExpression(begin));

    while (true)
    {
        auto next = peekNextToken();
        if (next->type == TokenType::LEF_SHIFT || next->type == TokenType::RIGHT_SHIFT)
        {
            begin = getNextToken();
            ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
            begin = getNextToken();
            ret->children.push_back(additiveExpression(begin));
        }
        else
            break;
    }
    return ret;
}

std::shared_ptr<Node> AST::relationalExpression(std::list<Token>::iterator begin)
{
    Node stmt(TokenType::RELATIONAL_EXPRESSION);
    std::shared_ptr<Node> ret = std::make_shared<Node>(std::move(stmt));
    ret->children.push_back(shiftExpression(begin));

    while (true)
    {
        auto next = peekNextToken();
        if (next->type == TokenType::LT || next->type == TokenType::GT ||
            next->type == TokenType::LTE || next->type == TokenType::GTE)
        {
            begin = getNextToken();
            ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
            begin = getNextToken();
            ret->children.push_back(shiftExpression(begin));
        }
        else
            break;
    }
    return ret;
}

std::shared_ptr<Node> AST::equalityExpression(std::list<Token>::iterator begin)
{
    Node stmt(TokenType::EQUALITY_EXPRESSION);
    std::shared_ptr<Node> ret = std::make_shared<Node>(std::move(stmt));
    ret->children.push_back(relationalExpression(begin));

    while (true)
    {
        auto next = peekNextToken();
        if (next->type == TokenType::EQ || next->type == TokenType::UNEQUAL)
        {
            begin = getNextToken();
            ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
            begin = getNextToken();
            ret->children.push_back(relationalExpression(begin));
        }
        else
            break;
    }
    return ret;
}

std::shared_ptr<Node> AST::andExpression(std::list<Token>::iterator begin)
{
    Node stmt(TokenType::AND_EXPRESSION);
    std::shared_ptr<Node> ret = std::make_shared<Node>(std::move(stmt));
    ret->children.push_back(equalityExpression(begin));

    while (peekNextToken()->type == TokenType::REFERENCE)
    {
        begin = getNextToken();
        ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
        begin = getNextToken();
        ret->children.push_back(equalityExpression(begin));
    }
    return ret;
}

std::shared_ptr<Node> AST::exclusiveOrExpression(std::list<Token>::iterator begin)
{
    Node stmt(TokenType::EXCLUSIVE_OR_EXPRESSION);
    std::shared_ptr<Node> ret = std::make_shared<Node>(std::move(stmt));
    ret->children.push_back(andExpression(begin));

    while (peekNextToken()->type == TokenType::CARET)
    {
        begin = getNextToken();
        ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
        begin = getNextToken();
        ret->children.push_back(andExpression(begin));
    }
    return ret;
}

std::shared_ptr<Node> AST::inclusiveOrExpression(std::list<Token>::iterator begin)
{
    Node stmt(TokenType::INCLUSIVE_OR_EXPRESSION);
    std::shared_ptr<Node> ret = std::make_shared<Node>(std::move(stmt));
    ret->children.push_back(exclusiveOrExpression(begin));

    while (peekNextToken()->type == TokenType::PIPE)
    {
        begin = getNextToken();
        ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
        begin = getNextToken();
        ret->children.push_back(exclusiveOrExpression(begin));
    }
    return ret;
}

std::shared_ptr<Node> AST::logicalAndExpression(std::list<Token>::iterator begin)
{
    Node stmt(TokenType::LOGICAL_AND_EXPRESSION);
    std::shared_ptr<Node> ret = std::make_shared<Node>(std::move(stmt));
    ret->children.push_back(inclusiveOrExpression(begin));

    while (peekNextToken()->type == TokenType::AND)
    {
        begin = getNextToken();
        ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
        begin = getNextToken();
        ret->children.push_back(inclusiveOrExpression(begin));
    }
    return ret;
}

std::shared_ptr<Node> AST::logicalOrExpression(std::list<Token>::iterator begin)
{
    Node stmt(TokenType::LOGICAL_OR_EXPRESSION);
    std::shared_ptr<Node> ret = std::make_shared<Node>(std::move(stmt));
    ret->children.push_back(logicalAndExpression(begin));

    while (peekNextToken()->type == TokenType::OR)
    {
        begin = getNextToken();
        ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
        begin = getNextToken();
        ret->children.push_back(logicalAndExpression(begin));
    }
    return ret;
}

std::shared_ptr<Node> AST::conditionalExpression(std::list<Token>::iterator begin)
{
    Node stmt(TokenType::CONDITIONAL_EXPRESSION);
    std::shared_ptr<Node> ret = std::make_shared<Node>(std::move(stmt));
    ret->children.push_back(logicalOrExpression(begin));

    if (peekNextToken()->type == TokenType::QUESTION)
    {
        begin = getNextToken();
        ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
        begin = getNextToken();
        ret->children.push_back(expression(begin));
        begin = getNextToken();
        if (begin->type == TokenType::COLON)
        {
            ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
            begin = getNextToken();
            ret->children.push_back(conditionalExpression(begin));
        }
        else
            loggedError.addError(lineNo, "Expected ':' in conditional expression");
    }
    return ret;
}

std::shared_ptr<Node> AST::assignmentExpression(std::list<Token>::iterator begin)
{
    Node stmt(TokenType::ASSIGNMENT_EXPRESSION);
    std::shared_ptr<Node> ret = std::make_shared<Node>(std::move(stmt));

    // Try to parse as conditional expression first
    ret->children.push_back(conditionalExpression(begin));

    // Check if there's an assignment operator following
    auto next = peekNextToken();
    if (assignOperator(next))
    {
        begin = getNextToken();
        ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
        begin = getNextToken();
        ret->children.push_back(assignmentExpression(begin));
    }
    return ret;
}

std::shared_ptr<Node> AST::expression(std::list<Token>::iterator begin)
{
    Node stmt(TokenType::EXPRESSION);
    std::shared_ptr<Node> ret = std::make_shared<Node>(std::move(stmt));
    ret->children.push_back(assignmentExpression(begin));

    while (peekNextToken()->type == TokenType::COMMA)
    {
        begin = getNextToken();
        ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
        begin = getNextToken();
        ret->children.push_back(assignmentExpression(begin));
    }
    return ret;
}

std::shared_ptr<Node> AST::constantExpression(std::list<Token>::iterator begin)
{
    Node stmt(TokenType::CONSTANT_EXPRESSION);
    std::shared_ptr<Node> ret = std::make_shared<Node>(std::move(stmt));
    ret->children.push_back(conditionalExpression(begin));
    return ret;
}

std::shared_ptr<Node> AST::typeName(std::list<Token>::iterator begin)
{
    Node stmt(TokenType::TYPE_NAME);
    std::shared_ptr<Node> ret = std::make_shared<Node>(std::move(stmt));
    ret->children.push_back(specifierQualifierList(begin));

    auto next = peekNextToken();
    if (next->type == TokenType::MUL || next->type == TokenType::L_SQR || next->type == TokenType::L_BR)
    {
        begin = getNextToken();
        ret->children.push_back(abstractDeclarator(begin));
    }
    return ret;
}

// C11 specific implementations
std::shared_ptr<Node> AST::genericSelection(std::list<Token>::iterator begin)
{
    Node stmt(TokenType::GENERIC_SELECTION);
    std::shared_ptr<Node> ret = std::make_shared<Node>(std::move(stmt));
    
    if (begin->type == TokenType::GENERIC)
    {
        ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
        begin = getNextToken();
        
        if (begin->type == TokenType::L_BR)
        {
            ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
            begin = getNextToken();
            ret->children.push_back(assignmentExpression(begin));
            begin = getNextToken();
            
            if (begin->type == TokenType::COMMA)
            {
                ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
                begin = getNextToken();
                ret->children.push_back(genericAssocList(begin));
                begin = getNextToken();
            }
            
            if (begin->type == TokenType::R_BR)
                ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
            else
                loggedError.addGrammarError(lineNo, "Expected ')' in generic selection");
        }
        else
            loggedError.addGrammarError(lineNo, "Expected '(' after '_Generic'");
    }
    return ret;
}

std::shared_ptr<Node> AST::genericAssocList(std::list<Token>::iterator begin)
{
    Node stmt(TokenType::GENERIC_ASSOC_LIST);
    std::shared_ptr<Node> ret = std::make_shared<Node>(std::move(stmt));
    
    ret->children.push_back(genericAssociation(begin));
    
    while (peekNextToken()->type == TokenType::COMMA)
    {
        begin = getNextToken();
        ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
        begin = getNextToken();
        ret->children.push_back(genericAssociation(begin));
    }
    return ret;
}

std::shared_ptr<Node> AST::genericAssociation(std::list<Token>::iterator begin)
{
    Node stmt(TokenType::GENERIC_ASSOCIATION);
    std::shared_ptr<Node> ret = std::make_shared<Node>(std::move(stmt));
    
    if (begin->type == TokenType::DEFAULT)
    {
        ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
        begin = getNextToken();
    }
    else
    {
        ret->children.push_back(typeName(begin));
        begin = getNextToken();
    }
    
    if (begin->type == TokenType::COLON)
    {
        ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
        begin = getNextToken();
        ret->children.push_back(assignmentExpression(begin));
    }
    else
        loggedError.addGrammarError(lineNo, "Expected ':' in generic association");
    
    return ret;
}

std::shared_ptr<Node> AST::staticAssertDeclaration(std::list<Token>::iterator begin)
{
    Node stmt(TokenType::STATIC_ASSERT_DECLARATION);
    std::shared_ptr<Node> ret = std::make_shared<Node>(std::move(stmt));
    
    if (begin->type == TokenType::STATIC_ASSERT)
    {
        ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
        begin = getNextToken();
        
        if (begin->type == TokenType::L_BR)
        {
            ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
            begin = getNextToken();
            ret->children.push_back(constantExpression(begin));
            begin = getNextToken();
            
            if (begin->type == TokenType::COMMA)
            {
                ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
                begin = getNextToken();
                
                if (begin->type == TokenType::STRING_LITERAL)
                {
                    ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
                    begin = getNextToken();
                }
                else
                    loggedError.addGrammarError(lineNo, "Expected string literal in static assertion");
            }
            
            if (begin->type == TokenType::R_BR)
            {
                ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
                begin = getNextToken();
            }
            else
                loggedError.addGrammarError(lineNo, "Expected ')' in static assertion");
            
            if (begin->type == TokenType::SEMI_COLON)
                ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
            else
                loggedError.addGrammarError(lineNo, "Expected ';' after static assertion");
        }
        else
            loggedError.addGrammarError(lineNo, "Expected '(' after '_Static_assert'");
    }
    return ret;
}

std::shared_ptr<Node> AST::alignmentSpecifier(std::list<Token>::iterator begin)
{
    Node stmt(TokenType::ALIGNMENT_SPECIFIER);
    std::shared_ptr<Node> ret = std::make_shared<Node>(std::move(stmt));
    
    if (begin->type == TokenType::ALIGNAS)
    {
        ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
        begin = getNextToken();
        
        if (begin->type == TokenType::L_BR)
        {
            ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
            begin = getNextToken();
            
            // Try to parse as type-name first, if it fails, parse as constant expression
            if (typeSpecifier(begin) || typeQualifier(begin))
            {
                ret->children.push_back(typeName(begin));
            }
            else
            {
                ret->children.push_back(constantExpression(begin));
            }
            begin = getNextToken();
            
            if (begin->type == TokenType::R_BR)
                ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
            else
                loggedError.addGrammarError(lineNo, "Expected ')' in alignment specifier");
        }
        else
            loggedError.addGrammarError(lineNo, "Expected '(' after '_Alignas'");
    }
    return ret;
}

std::shared_ptr<Node> AST::atomicTypeSpecifier(std::list<Token>::iterator begin)
{
    Node stmt(TokenType::ATOMIC_TYPE_SPECIFIER);
    std::shared_ptr<Node> ret = std::make_shared<Node>(std::move(stmt));
    
    if (begin->type == TokenType::ATOMIC)
    {
        ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
        begin = getNextToken();
        
        if (begin->type == TokenType::L_BR)
        {
            ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
            begin = getNextToken();
            ret->children.push_back(typeName(begin));
            begin = getNextToken();
            
            if (begin->type == TokenType::R_BR)
                ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
            else
                loggedError.addGrammarError(lineNo, "Expected ')' in atomic type specifier");
        }
        else
        {
            // If no '(', treat as type qualifier not type specifier
            ungetToken();
        }
    }
    return ret;
}

void AST::printBranches(const std::vector<bool> &isLast, std::ostream &os)
{
    for (size_t i = 0; i < isLast.size() - 1; ++i)
    {
        if (!isLast[i])
        {
            os << "│   ";
        }
        else
        {
            os << "    ";
        }
    }
}

void AST::traverseTree(std::shared_ptr<Node> node, std::vector<bool> &isLast, std::ostream &os)
{
    if (!node)
        return;

    // Print branches for current level
    if (!isLast.empty())
    {
        printBranches(isLast, os);
        if (isLast.back())
        {
            os << "└── ";
        }
        else
        {
            os << "├── ";
        }
    }
    os << "Token: " << t(node->t.type) << " ";
    os << "lexeme: " << node->t.lexeme << std::endl;

    // Process children
    for (size_t i = 0; i < node->children.size(); ++i)
    {
        isLast.push_back(i == node->children.size() - 1);
        traverseTree(node->children[i], isLast, os);
        isLast.pop_back();
    }
}
void AST::printAST(std::ostream &os)
{

    if (!root)
        return;
    std::vector<bool> isLast;
    traverseTree(root, isLast, os);
}
