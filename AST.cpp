#include "AST.hpp"

AST::AST(const std::string &path, Error &e) : Scanner(path, e), root(std::make_shared<Node>(TokenType::TRANSLATION_UNIT))
{
    root->t.lexeme = path;
    root = translationUnit();
}

// Helper functions
bool AST::matchToken(TokenType expected)
{
    auto token = peekNextToken();
    if (token == symbolTable.end())
        return false;
    return token->type == expected;
}

bool AST::lookahead(TokenType expected)
{
    return matchToken(expected);
}

void AST::expectToken(TokenType expected, const std::string &errorMsg)
{
    auto token = getNextToken();
    if (token->type != expected)
    {
        if (errorMsg.empty())
            loggedError.addError(lineNo, "Unexpected token");
        else
            loggedError.addError(lineNo, errorMsg);
    }
}

// Top level parsing
std::shared_ptr<Node> AST::translationUnit()
{
    auto node = std::make_shared<Node>(TokenType::TRANSLATION_UNIT);
    node->t.lexeme = root->t.lexeme;

    while (!matchToken(TokenType::END))
    {
        if (matchToken(TokenType::INCLUDE))
        {
            node->children.push_back(includeStmt());
        }
        else
        {
            node->children.push_back(externalDeclaration());
        }
    }

    return node;
}

std::shared_ptr<Node> AST::externalDeclaration()
{
    auto node = std::make_shared<Node>(TokenType::DECLARATION_SPECIFIERS);

    // Check if it's a function definition
    auto lookaheadToken = peekNextToken();

    // Try to parse declaration specifiers
    if (storageClassSpecifier(lookaheadToken) || typeSpecifier(lookaheadToken) ||
        typeQualifier(lookaheadToken) || isTypeName(lookaheadToken->lexeme))
    {
        // Check if followed by declarator and '{' or ';'
        auto tempToken = lookaheadToken;
        int depth = 0;
        bool foundDeclarator = false;
        bool foundLBrace = false;

        // Simple lookahead to determine if function definition
        while (tempToken != symbolTable.end() && tempToken->type != TokenType::END)
        {
            if (tempToken->type == TokenType::L_BR)
                depth++;
            else if (tempToken->type == TokenType::R_BR)
                depth--;
            else if (depth == 0 && tempToken->type == TokenType::L_CUR)
            {
                foundLBrace = true;
                foundDeclarator = true;
                break;
            }
            else if (depth == 0 && tempToken->type == TokenType::SEMI_COLON)
            {
                break;
            }

            appendList(symbolTable);
            tempToken = peekNextToken();
            if (tempToken == symbolTable.end())
                appendList(symbolTable);
            tempToken = peekNextToken();
        }

        if (foundLBrace && foundDeclarator)
        {
            // It's a function definition
            return functionDefinition();
        }
    }

    // Otherwise it's a declaration
    return declaration();
}

std::shared_ptr<Node> AST::functionDefinition()
{
    auto node = std::make_shared<Node>(TokenType::DECLARATION_SPECIFIERS);

    node->children.push_back(declarationSpecifiers());

    auto declNode = declarator();
    node->children.push_back(declNode);

    // Optional declaration list (K&R style)
    if (matchToken(TokenType::INT_TYPE) || matchToken(TokenType::CHAR_TYPE) ||
        matchToken(TokenType::VOID) || storageClassSpecifier(peekNextToken()))
    {
        node->children.push_back(declarationList());
    }

    node->children.push_back(compoundStatement());

    return node;
}

std::shared_ptr<Node> AST::declarationList()
{
    auto node = std::make_shared<Node>(TokenType::DECLARATION_SPECIFIERS);

    node->children.push_back(declaration());

    while (matchToken(TokenType::INT_TYPE) || matchToken(TokenType::CHAR_TYPE) ||
           matchToken(TokenType::VOID) || storageClassSpecifier(peekNextToken()))
    {
        node->children.push_back(declaration());
    }

    return node;
}

std::shared_ptr<Node> AST::parsingFile(std::shared_ptr<Node> root)
{
    return translationUnit();
}

// Expression parsing - following operator precedence from grammar.y
std::shared_ptr<Node> AST::expression()
{
    auto node = std::make_shared<Node>(TokenType::ASSIGNMENT_EXP);

    node->children.push_back(assignmentExpression());

    while (matchToken(TokenType::COMMA))
    {
        auto commaToken = getNextToken();
        node->children.push_back(std::make_shared<Node>(*commaToken));
        node->children.push_back(assignmentExpression());
    }

    return node;
}

std::shared_ptr<Node> AST::assignmentExpression()
{
    // Check if it starts with unary operator tokens (for assignment)
    auto lookaheadToken = peekNextToken();
    auto secondToken = symbolTable.end();
    if (lookaheadToken != symbolTable.end())
    {
        auto temp = std::next(lookaheadToken);
        if (temp != symbolTable.end())
            secondToken = temp;
    }

    // If followed by assignment operator, parse assignment
    if (lookaheadToken->type == TokenType::ID || lookaheadToken->type == TokenType::MUL ||
        lookaheadToken->type == TokenType::INC || lookaheadToken->type == TokenType::DEC ||
        (secondToken != symbolTable.end() && assignOperator(secondToken)))
    {
        // Try to parse as assignment
        auto unary = unaryExpression();
        if (assignOperator(peekNextToken()))
        {
            auto assignNode = std::make_shared<Node>(TokenType::ASSIGNMENT_EXP);
            assignNode->children.push_back(unary);
            assignNode->children.push_back(assignmentOperator());
            assignNode->children.push_back(assignmentExpression());
            return assignNode;
        }
        // Otherwise return conditional
        return conditionalExpression();
    }

    return conditionalExpression();
}

std::shared_ptr<Node> AST::assignmentOperator()
{
    auto token = getNextToken();
    if (assignOperator(token))
    {
        return std::make_shared<Node>(*token);
    }
    loggedError.addError(lineNo, "Expected assignment operator");
    return std::make_shared<Node>(TokenType::ASSIGN);
}

std::shared_ptr<Node> AST::conditionalExpression()
{
    auto node = logicalOrExpression();

    // Check for ternary operator '?'
    auto token = peekNextToken();
    if (token != symbolTable.end() && token->lexeme == "?")
    {
        auto questionToken = getNextToken();
        auto questionNode = std::make_shared<Node>(Token(TokenType::ASSIGN, "?"));

        auto exprNode = expression();
        expectToken(TokenType::COLON, "Expected ':' after '?'");
        auto colonToken = getNextToken();
        auto colonNode = std::make_shared<Node>(*colonToken);

        auto condNode = std::make_shared<Node>(TokenType::ASSIGNMENT_EXP);
        condNode->children.push_back(node);
        condNode->children.push_back(questionNode);
        condNode->children.push_back(exprNode);
        condNode->children.push_back(colonNode);
        condNode->children.push_back(conditionalExpression());

        return condNode;
    }

    return node;
}

std::shared_ptr<Node> AST::logicalOrExpression()
{
    auto node = logicalAndExpression();

    while (matchToken(TokenType::OR))
    {
        auto opToken = getNextToken();
        auto opNode = std::make_shared<Node>(*opToken);
        auto right = logicalAndExpression();

        auto exprNode = std::make_shared<Node>(TokenType::ASSIGNMENT_EXP);
        exprNode->children.push_back(node);
        exprNode->children.push_back(opNode);
        exprNode->children.push_back(right);

        node = exprNode;
    }

    return node;
}

std::shared_ptr<Node> AST::logicalAndExpression()
{
    auto node = inclusiveOrExpression();

    while (matchToken(TokenType::AND))
    {
        auto opToken = getNextToken();
        auto opNode = std::make_shared<Node>(*opToken);
        auto right = inclusiveOrExpression();

        auto exprNode = std::make_shared<Node>(TokenType::ASSIGNMENT_EXP);
        exprNode->children.push_back(node);
        exprNode->children.push_back(opNode);
        exprNode->children.push_back(right);

        node = exprNode;
    }

    return node;
}

std::shared_ptr<Node> AST::inclusiveOrExpression()
{
    auto node = exclusiveOrExpression();

    // Check for bitwise OR '|' (not logical OR '||')
    while (true)
    {
        auto token = peekNextToken();
        if (token == symbolTable.end())
            break;
        // Check if it's a single '|' (bitwise OR)
        if (token->lexeme == "|" && (std::next(token) == symbolTable.end() ||
                                     std::next(token)->lexeme != "|"))
        {
            getNextToken(); // Consume the operator
            auto opNode = std::make_shared<Node>(Token(TokenType::OR, "|"));
            auto right = exclusiveOrExpression();

            auto exprNode = std::make_shared<Node>(TokenType::ASSIGNMENT_EXP);
            exprNode->children.push_back(node);
            exprNode->children.push_back(opNode);
            exprNode->children.push_back(right);

            node = exprNode;
        }
        else
            break;
    }

    return node;
}

std::shared_ptr<Node> AST::exclusiveOrExpression()
{
    auto node = andExpression();

    // Check for XOR operator '^' (not '^=')
    while (true)
    {
        auto token = peekNextToken();
        if (token == symbolTable.end())
            break;
        // Check if it's a single '^' (XOR)
        if (token->lexeme == "^" && (std::next(token) == symbolTable.end() ||
                                     std::next(token)->lexeme != "="))
        {
            getNextToken(); // Consume the operator
            auto opNode = std::make_shared<Node>(Token(TokenType::XOR_ASSIGN, "^"));
            auto right = andExpression();

            auto exprNode = std::make_shared<Node>(TokenType::ASSIGNMENT_EXP);
            exprNode->children.push_back(node);
            exprNode->children.push_back(opNode);
            exprNode->children.push_back(right);

            node = exprNode;
        }
        else
            break;
    }

    return node;
}

std::shared_ptr<Node> AST::andExpression()
{
    auto node = equalityExpression();

    // Check for bitwise AND '&' (not '&&' or '&=')
    while (true)
    {
        auto token = peekNextToken();
        if (token == symbolTable.end())
            break;
        // Check if it's a single '&' (bitwise AND)
        if (token->lexeme == "&" && (std::next(token) == symbolTable.end() ||
                                     (std::next(token)->lexeme != "&" && std::next(token)->lexeme != "=")))
        {
            auto opToken = getNextToken();
            auto opNode = std::make_shared<Node>(*opToken);
            auto right = equalityExpression();

            auto exprNode = std::make_shared<Node>(TokenType::ASSIGNMENT_EXP);
            exprNode->children.push_back(node);
            exprNode->children.push_back(opNode);
            exprNode->children.push_back(right);

            node = exprNode;
        }
        else
            break;
    }

    return node;
}

std::shared_ptr<Node> AST::equalityExpression()
{
    auto node = relationalExpression();

    while (matchToken(TokenType::EQ) || matchToken(TokenType::UNEQUAL))
    {
        auto opToken = getNextToken();
        auto opNode = std::make_shared<Node>(*opToken);
        auto right = relationalExpression();

        auto exprNode = std::make_shared<Node>(TokenType::ASSIGNMENT_EXP);
        exprNode->children.push_back(node);
        exprNode->children.push_back(opNode);
        exprNode->children.push_back(right);

        node = exprNode;
    }

    return node;
}

std::shared_ptr<Node> AST::relationalExpression()
{
    auto node = shiftExpression();

    while (matchToken(TokenType::LT) || matchToken(TokenType::GT) ||
           matchToken(TokenType::LTE) || matchToken(TokenType::GTE))
    {
        auto opToken = getNextToken();
        auto opNode = std::make_shared<Node>(*opToken);
        auto right = shiftExpression();

        auto exprNode = std::make_shared<Node>(TokenType::ASSIGNMENT_EXP);
        exprNode->children.push_back(node);
        exprNode->children.push_back(opNode);
        exprNode->children.push_back(right);

        node = exprNode;
    }

    return node;
}

std::shared_ptr<Node> AST::shiftExpression()
{
    auto node = additiveExpression();

    while (matchToken(TokenType::LEF_SHIFT) || matchToken(TokenType::RIGHT_SHIFT))
    {
        auto opToken = getNextToken();
        auto opNode = std::make_shared<Node>(*opToken);
        auto right = additiveExpression();

        auto exprNode = std::make_shared<Node>(TokenType::ASSIGNMENT_EXP);
        exprNode->children.push_back(node);
        exprNode->children.push_back(opNode);
        exprNode->children.push_back(right);

        node = exprNode;
    }

    return node;
}

std::shared_ptr<Node> AST::additiveExpression()
{
    auto node = multiplicativeExpression();

    while (matchToken(TokenType::PLUS) || matchToken(TokenType::MINUS))
    {
        auto opToken = getNextToken();
        auto opNode = std::make_shared<Node>(*opToken);
        auto right = multiplicativeExpression();

        auto exprNode = std::make_shared<Node>(TokenType::ASSIGNMENT_EXP);
        exprNode->children.push_back(node);
        exprNode->children.push_back(opNode);
        exprNode->children.push_back(right);

        node = exprNode;
    }

    return node;
}

std::shared_ptr<Node> AST::multiplicativeExpression()
{
    auto node = castExpression();

    while (matchToken(TokenType::MUL) || matchToken(TokenType::DIV) || matchToken(TokenType::MOD))
    {
        auto opToken = getNextToken();
        auto opNode = std::make_shared<Node>(*opToken);
        auto right = castExpression();

        auto exprNode = std::make_shared<Node>(TokenType::ASSIGNMENT_EXP);
        exprNode->children.push_back(node);
        exprNode->children.push_back(opNode);
        exprNode->children.push_back(right);

        node = exprNode;
    }

    return node;
}

std::shared_ptr<Node> AST::castExpression()
{
    // Check for cast: '(' type_name ')'
    if (matchToken(TokenType::L_BR))
    {
        auto lookaheadToken = peekNextToken();
        // Check if next tokens form a type name
        if (typeSpecifier(lookaheadToken) || typeQualifier(lookaheadToken) ||
            storageClassSpecifier(lookaheadToken) || isTypeName(lookaheadToken->lexeme))
        {
            auto lbrToken = getNextToken();
            auto typeNameNode = parseTypeName();
            if (matchToken(TokenType::R_BR))
            {
                auto rbrToken = getNextToken();
                auto castNode = std::make_shared<Node>(TokenType::ASSIGNMENT_EXP);
                castNode->children.push_back(std::make_shared<Node>(*lbrToken));
                castNode->children.push_back(typeNameNode);
                castNode->children.push_back(std::make_shared<Node>(*rbrToken));
                castNode->children.push_back(castExpression());
                return castNode;
            }
            else
            {
                ungetToken();
                ungetToken();
                ungetToken();
            }
        }
    }

    return unaryExpression();
}

std::shared_ptr<Node> AST::unaryExpression()
{
    // Check for prefix operators
    if (matchToken(TokenType::INC) || matchToken(TokenType::DEC))
    {
        auto opToken = getNextToken();
        auto opNode = std::make_shared<Node>(*opToken);
        auto exprNode = std::make_shared<Node>(TokenType::ASSIGNMENT_EXP);
        exprNode->children.push_back(opNode);
        exprNode->children.push_back(unaryExpression());
        return exprNode;
    }

    // Check for sizeof/alignof
    if (matchToken(TokenType::SIZEOF))
    {
        auto sizeofToken = getNextToken();
        auto sizeofNode = std::make_shared<Node>(*sizeofToken);

        if (matchToken(TokenType::L_BR))
        {
            auto lbr = getNextToken();
            auto typeNode = parseTypeName();
            expectToken(TokenType::R_BR, "Expected ')' after type name in sizeof");
            auto rbr = getNextToken();

            auto exprNode = std::make_shared<Node>(TokenType::ASSIGNMENT_EXP);
            exprNode->children.push_back(sizeofNode);
            exprNode->children.push_back(std::make_shared<Node>(*lbr));
            exprNode->children.push_back(typeNode);
            exprNode->children.push_back(std::make_shared<Node>(*rbr));
            return exprNode;
        }
        else
        {
            auto exprNode = std::make_shared<Node>(TokenType::ASSIGNMENT_EXP);
            exprNode->children.push_back(sizeofNode);
            exprNode->children.push_back(unaryExpression());
            return exprNode;
        }
    }

    // Check for unary operators
    if (matchToken(TokenType::REFERENCE) || matchToken(TokenType::MUL) ||
        matchToken(TokenType::PLUS) || matchToken(TokenType::MINUS) ||
        matchToken(TokenType::TILDE) || matchToken(TokenType::NOT))
    {
        auto opNode = unaryOperator();
        auto exprNode = std::make_shared<Node>(TokenType::ASSIGNMENT_EXP);
        exprNode->children.push_back(opNode);
        exprNode->children.push_back(castExpression());
        return exprNode;
    }

    return postfixExpression();
}

std::shared_ptr<Node> AST::unaryOperator()
{
    auto token = getNextToken();
    return std::make_shared<Node>(*token);
}

std::shared_ptr<Node> AST::postfixExpression()
{
    auto node = primaryExpression();

    while (true)
    {
        // Array subscript
        if (matchToken(TokenType::L_SQR))
        {
            auto lsqr = getNextToken();
            auto expr = expression();
            expectToken(TokenType::R_SQR, "Expected ']' after array subscript");
            auto rsqr = getNextToken();

            auto exprNode = std::make_shared<Node>(TokenType::ASSIGNMENT_EXP);
            exprNode->children.push_back(node);
            exprNode->children.push_back(std::make_shared<Node>(*lsqr));
            exprNode->children.push_back(expr);
            exprNode->children.push_back(std::make_shared<Node>(*rsqr));
            node = exprNode;
        }
        // Function call
        else if (matchToken(TokenType::L_BR))
        {
            auto lbr = getNextToken();

            if (matchToken(TokenType::R_BR))
            {
                auto rbr = getNextToken();
                auto exprNode = std::make_shared<Node>(TokenType::ASSIGNMENT_EXP);
                exprNode->children.push_back(node);
                exprNode->children.push_back(std::make_shared<Node>(*lbr));
                exprNode->children.push_back(std::make_shared<Node>(*rbr));
                node = exprNode;
            }
            else
            {
                auto argList = argumentExpressionList();
                expectToken(TokenType::R_BR, "Expected ')' after argument list");
                auto rbr = getNextToken();

                auto exprNode = std::make_shared<Node>(TokenType::ASSIGNMENT_EXP);
                exprNode->children.push_back(node);
                exprNode->children.push_back(std::make_shared<Node>(*lbr));
                exprNode->children.push_back(argList);
                exprNode->children.push_back(std::make_shared<Node>(*rbr));
                node = exprNode;
            }
        }
        // Member access
        else if (matchToken(TokenType::DOT))
        {
            auto dot = getNextToken();
            expectToken(TokenType::ID, "Expected identifier after '.'");
            auto id = getNextToken();

            auto exprNode = std::make_shared<Node>(TokenType::ASSIGNMENT_EXP);
            exprNode->children.push_back(node);
            exprNode->children.push_back(std::make_shared<Node>(*dot));
            exprNode->children.push_back(std::make_shared<Node>(*id));
            node = exprNode;
        }
        // Pointer member access
        else if (matchToken(TokenType::ARRORW))
        {
            auto arrow = getNextToken();
            expectToken(TokenType::ID, "Expected identifier after '->'");
            auto id = getNextToken();

            auto exprNode = std::make_shared<Node>(TokenType::ASSIGNMENT_EXP);
            exprNode->children.push_back(node);
            exprNode->children.push_back(std::make_shared<Node>(*arrow));
            exprNode->children.push_back(std::make_shared<Node>(*id));
            node = exprNode;
        }
        // Postfix increment/decrement
        else if (matchToken(TokenType::INC) || matchToken(TokenType::DEC))
        {
            auto op = getNextToken();
            auto exprNode = std::make_shared<Node>(TokenType::ASSIGNMENT_EXP);
            exprNode->children.push_back(node);
            exprNode->children.push_back(std::make_shared<Node>(*op));
            node = exprNode;
        }
        else
        {
            break;
        }
    }

    return node;
}

std::shared_ptr<Node> AST::argumentExpressionList()
{
    auto node = std::make_shared<Node>(TokenType::PARAMETER_LIST);

    node->children.push_back(assignmentExpression());

    while (matchToken(TokenType::COMMA))
    {
        auto comma = getNextToken();
        node->children.push_back(std::make_shared<Node>(*comma));
        node->children.push_back(assignmentExpression());
    }

    return node;
}

std::shared_ptr<Node> AST::primaryExpression()
{
    if (matchToken(TokenType::ID))
    {
        auto token = getNextToken();
        return std::make_shared<Node>(*token);
    }

    if (matchToken(TokenType::CONSTANT))
    {
        return constant();
    }

    if (matchToken(TokenType::STRING_LITERAL))
    {
        return stringLiteral();
    }

    if (matchToken(TokenType::L_BR))
    {
        auto lbr = getNextToken();
        auto expr = expression();
        expectToken(TokenType::R_BR, "Expected ')' after expression");
        auto rbr = getNextToken();

        auto node = std::make_shared<Node>(TokenType::ASSIGNMENT_EXP);
        node->children.push_back(std::make_shared<Node>(*lbr));
        node->children.push_back(expr);
        node->children.push_back(std::make_shared<Node>(*rbr));
        return node;
    }

    loggedError.addError(lineNo, "Expected primary expression");
    return std::make_shared<Node>(TokenType::END);
}

std::shared_ptr<Node> AST::constant()
{
    if (matchToken(TokenType::CONSTANT))
    {
        auto token = getNextToken();
        return std::make_shared<Node>(*token);
    }

    loggedError.addError(lineNo, "Expected constant");
    return std::make_shared<Node>(TokenType::CONSTANT);
}

std::shared_ptr<Node> AST::stringLiteral()
{
    if (matchToken(TokenType::STRING_LITERAL))
    {
        auto token = getNextToken();
        return std::make_shared<Node>(*token);
    }

    loggedError.addError(lineNo, "Expected string literal");
    return std::make_shared<Node>(TokenType::STRING_LITERAL);
}

std::shared_ptr<Node> AST::genericSelection()
{
    auto node = std::make_shared<Node>(TokenType::ASSIGNMENT_EXP);

    // GENERIC token would need to be added to scanner
    // For now, skip implementation
    return node;
}

std::shared_ptr<Node> AST::genericAssocList()
{
    auto node = std::make_shared<Node>(TokenType::PARAMETER_LIST);
    node->children.push_back(genericAssociation());

    while (matchToken(TokenType::COMMA))
    {
        auto comma = getNextToken();
        node->children.push_back(std::make_shared<Node>(*comma));
        node->children.push_back(genericAssociation());
    }

    return node;
}

std::shared_ptr<Node> AST::genericAssociation()
{
    auto node = std::make_shared<Node>(TokenType::PARAMETER_DECLARATION);

    if (matchToken(TokenType::DEFAULT))
    {
        auto defToken = getNextToken();
        node->children.push_back(std::make_shared<Node>(*defToken));
    }
    else
    {
        node->children.push_back(parseTypeName());
    }

    expectToken(TokenType::COLON, "Expected ':' in generic association");
    auto colon = getNextToken();
    node->children.push_back(std::make_shared<Node>(*colon));
    node->children.push_back(assignmentExpression());

    return node;
}

std::shared_ptr<Node> AST::constantExpression()
{
    return conditionalExpression();
}

// Declaration parsing
std::shared_ptr<Node> AST::declaration()
{
    auto node = std::make_shared<Node>(TokenType::DECLARATION_SPECIFIERS);

    if (matchToken(TokenType::STATIC) && peekNextToken()->lexeme == "_" &&
        std::next(peekNextToken()) != symbolTable.end() &&
        std::next(peekNextToken())->lexeme == "Assert")
    {
        return staticAssertDeclaration();
    }

    node->children.push_back(declarationSpecifiers());

    if (matchToken(TokenType::SEMI_COLON))
    {
        auto semi = getNextToken();
        node->children.push_back(std::make_shared<Node>(*semi));
        return node;
    }

    node->children.push_back(initDeclaratorList());
    expectToken(TokenType::SEMI_COLON, "Expected ';' after declaration");
    auto semi = getNextToken();
    node->children.push_back(std::make_shared<Node>(*semi));

    return node;
}

std::shared_ptr<Node> AST::declarationSpecifiers()
{
    auto node = std::make_shared<Node>(TokenType::DECLARATION_SPECIFIERS);

    while (true)
    {
        auto token = peekNextToken();
        if (token == symbolTable.end())
            break;

        bool matched = false;

        if (storageClassSpecifier(token))
        {
            node->children.push_back(storageClassSpecifier());
            matched = true;
        }
        else if (typeSpecifier(token))
        {
            node->children.push_back(typeSpecifier());
            matched = true;
        }
        else if (typeQualifier(token))
        {
            node->children.push_back(typeQualifier());
            matched = true;
        }
        else if (isTypeName(token->lexeme))
        {
            auto typedefToken = getNextToken();
            node->children.push_back(std::make_shared<Node>(*typedefToken));
            matched = true;
        }

        if (!matched)
            break;
    }

    return node;
}

std::shared_ptr<Node> AST::storageClassSpecifier()
{
    auto token = getNextToken();
    if (storageClassSpecifier(token))
    {
        if (token->type == TokenType::TYPEDEF)
        {
            // Track typedef names
            // This would need more sophisticated tracking
        }
        return std::make_shared<Node>(*token);
    }

    loggedError.addError(lineNo, "Expected storage class specifier");
    return std::make_shared<Node>(TokenType::TYPEDEF);
}

std::shared_ptr<Node> AST::typeSpecifier()
{
    auto token = peekNextToken();

    if (token->type == TokenType::STRUCT || token->type == TokenType::UNION)
    {
        return structOrUnionSpecifier();
    }

    if (token->type == TokenType::ENUM)
    {
        return enumSpecifier();
    }

    if (typeSpecifier(token))
    {
        token = getNextToken();
        return std::make_shared<Node>(*token);
    }

    if (isTypeName(token->lexeme))
    {
        token = getNextToken();
        return std::make_shared<Node>(*token);
    }

    loggedError.addError(lineNo, "Expected type specifier");
    return std::make_shared<Node>(TokenType::INT_TYPE);
}

std::shared_ptr<Node> AST::typeQualifier()
{
    auto token = getNextToken();
    if (typeQualifier(token))
    {
        return std::make_shared<Node>(*token);
    }

    loggedError.addError(lineNo, "Expected type qualifier");
    return std::make_shared<Node>(TokenType::CONST);
}

std::shared_ptr<Node> AST::functionSpecifier()
{
    auto token = peekNextToken();
    // Note: INLINE and NORETURN would need to be added to TokenType
    // For now, return a placeholder
    if (token != symbolTable.end())
    {
        token = getNextToken();
        return std::make_shared<Node>(*token);
    }

    loggedError.addError(lineNo, "Expected function specifier");
    return std::make_shared<Node>(TokenType::VOID);
}

std::shared_ptr<Node> AST::alignmentSpecifier()
{
    auto node = std::make_shared<Node>(TokenType::DECLARATION_SPECIFIERS);
    // ALIGNAS would need to be added to scanner
    return node;
}

std::shared_ptr<Node> AST::initDeclaratorList()
{
    auto node = std::make_shared<Node>(TokenType::INIT_DECLARATOR);

    node->children.push_back(initDeclarator());

    while (matchToken(TokenType::COMMA))
    {
        auto comma = getNextToken();
        node->children.push_back(std::make_shared<Node>(*comma));
        node->children.push_back(initDeclarator());
    }

    return node;
}

std::shared_ptr<Node> AST::initDeclarator()
{
    auto node = std::make_shared<Node>(TokenType::INIT_DECLARATOR);

    node->children.push_back(declarator());

    if (matchToken(TokenType::ASSIGN))
    {
        auto assign = getNextToken();
        node->children.push_back(std::make_shared<Node>(*assign));
        node->children.push_back(initializer());
    }

    return node;
}

std::shared_ptr<Node> AST::declarator()
{
    auto node = std::make_shared<Node>(TokenType::DECLARATOR);

    if (matchToken(TokenType::MUL))
    {
        node->children.push_back(pointer());
    }

    node->children.push_back(directDeclarator());

    return node;
}

std::shared_ptr<Node> AST::directDeclarator()
{
    auto node = std::make_shared<Node>(TokenType::DIRECT_DECLARATOR);

    if (matchToken(TokenType::ID))
    {
        auto id = getNextToken();
        node->children.push_back(std::make_shared<Node>(*id));
    }
    else if (matchToken(TokenType::L_BR))
    {
        auto lbr = getNextToken();
        node->children.push_back(std::make_shared<Node>(*lbr));
        node->children.push_back(declarator());
        expectToken(TokenType::R_BR, "Expected ')' after declarator");
        auto rbr = getNextToken();
        node->children.push_back(std::make_shared<Node>(*rbr));
    }
    else
    {
        loggedError.addError(lineNo, "Expected identifier or '(' in direct declarator");
        return node;
    }

    // Parse array/function suffixes
    while (true)
    {
        // Array declarator
        if (matchToken(TokenType::L_SQR))
        {
            auto lsqr = getNextToken();
            node->children.push_back(std::make_shared<Node>(*lsqr));

            if (matchToken(TokenType::R_SQR))
            {
                auto rsqr = getNextToken();
                node->children.push_back(std::make_shared<Node>(*rsqr));
            }
            else if (matchToken(TokenType::STATIC))
            {
                auto stat = getNextToken();
                node->children.push_back(std::make_shared<Node>(*stat));

                if (typeQualifier(peekNextToken()))
                {
                    node->children.push_back(typeQualifierList());
                }

                node->children.push_back(assignmentExpression());
                expectToken(TokenType::R_SQR, "Expected ']'");
                auto rsqr = getNextToken();
                node->children.push_back(std::make_shared<Node>(*rsqr));
            }
            else if (typeQualifier(peekNextToken()))
            {
                node->children.push_back(typeQualifierList());

                if (matchToken(TokenType::STATIC))
                {
                    auto stat = getNextToken();
                    node->children.push_back(std::make_shared<Node>(*stat));
                    node->children.push_back(assignmentExpression());
                }
                else if (matchToken(TokenType::MUL))
                {
                    auto mul = getNextToken();
                    node->children.push_back(std::make_shared<Node>(*mul));
                }
                else
                {
                    node->children.push_back(assignmentExpression());
                }

                expectToken(TokenType::R_SQR, "Expected ']'");
                auto rsqr = getNextToken();
                node->children.push_back(std::make_shared<Node>(*rsqr));
            }
            else
            {
                node->children.push_back(assignmentExpression());
                expectToken(TokenType::R_SQR, "Expected ']'");
                auto rsqr = getNextToken();
                node->children.push_back(std::make_shared<Node>(*rsqr));
            }
        }
        // Function declarator
        else if (matchToken(TokenType::L_BR))
        {
            auto lbr = getNextToken();
            node->children.push_back(std::make_shared<Node>(*lbr));

            if (matchToken(TokenType::R_BR))
            {
                auto rbr = getNextToken();
                node->children.push_back(std::make_shared<Node>(*rbr));
            }
            else if (matchToken(TokenType::ID))
            {
                // Old style (K&R) parameter list
                node->children.push_back(identifierList());
                expectToken(TokenType::R_BR, "Expected ')'");
                auto rbr = getNextToken();
                node->children.push_back(std::make_shared<Node>(*rbr));
            }
            else
            {
                node->children.push_back(parameterTypeList());
                expectToken(TokenType::R_BR, "Expected ')'");
                auto rbr = getNextToken();
                node->children.push_back(std::make_shared<Node>(*rbr));
            }
        }
        else
        {
            break;
        }
    }

    return node;
}

std::shared_ptr<Node> AST::pointer()
{
    auto node = std::make_shared<Node>(TokenType::POINTER);

    expectToken(TokenType::MUL, "Expected '*' in pointer");
    auto mul = getNextToken();
    node->children.push_back(std::make_shared<Node>(*mul));

    if (typeQualifier(peekNextToken()))
    {
        node->children.push_back(typeQualifierList());
    }

    if (matchToken(TokenType::MUL))
    {
        node->children.push_back(pointer());
    }

    return node;
}

std::shared_ptr<Node> AST::typeQualifierList()
{
    auto node = std::make_shared<Node>(TokenType::TYPE_QUALIFIER_LIST);

    while (typeQualifier(peekNextToken()))
    {
        node->children.push_back(typeQualifier());
    }

    return node;
}

std::shared_ptr<Node> AST::parameterTypeList()
{
    auto node = std::make_shared<Node>(TokenType::PARAMETER_TYPE_LIST);

    node->children.push_back(parameterList());

    if (matchToken(TokenType::COMMA))
    {
        auto comma = getNextToken();
        node->children.push_back(std::make_shared<Node>(*comma));
        expectToken(TokenType::ELLIPSIS, "Expected '...' after ','");
        auto ellipsis = getNextToken();
        node->children.push_back(std::make_shared<Node>(*ellipsis));
    }

    return node;
}

std::shared_ptr<Node> AST::parameterList()
{
    auto node = std::make_shared<Node>(TokenType::PARAMETER_LIST);

    node->children.push_back(parameterDeclaration());

    while (matchToken(TokenType::COMMA))
    {
        auto comma = getNextToken();
        node->children.push_back(std::make_shared<Node>(*comma));
        node->children.push_back(parameterDeclaration());
    }

    return node;
}

std::shared_ptr<Node> AST::parameterDeclaration()
{
    auto node = std::make_shared<Node>(TokenType::PARAMETER_DECLARATION);

    node->children.push_back(declarationSpecifiers());

    if (matchToken(TokenType::ID) || matchToken(TokenType::MUL) || matchToken(TokenType::L_BR) || matchToken(TokenType::L_SQR))
    {
        auto token = peekNextToken();
        if (token->type == TokenType::ID)
        {
            node->children.push_back(declarator());
        }
        else
        {
            node->children.push_back(abstractDeclarator());
        }
    }

    return node;
}

std::shared_ptr<Node> AST::identifierList()
{
    auto node = std::make_shared<Node>(TokenType::IDENTIFIER_LIST);

    expectToken(TokenType::ID, "Expected identifier");
    auto id = getNextToken();
    node->children.push_back(std::make_shared<Node>(*id));

    while (matchToken(TokenType::COMMA))
    {
        auto comma = getNextToken();
        node->children.push_back(std::make_shared<Node>(*comma));
        expectToken(TokenType::ID, "Expected identifier");
        id = getNextToken();
        node->children.push_back(std::make_shared<Node>(*id));
    }

    return node;
}

std::shared_ptr<Node> AST::parseTypeName()
{
    auto node = std::make_shared<Node>(TokenType::DECLARATION_SPECIFIERS);

    node->children.push_back(specifierQualifierList());

    if (matchToken(TokenType::MUL) || matchToken(TokenType::L_BR) || matchToken(TokenType::L_SQR))
    {
        node->children.push_back(abstractDeclarator());
    }

    return node;
}

std::shared_ptr<Node> AST::abstractDeclarator()
{
    auto node = std::make_shared<Node>(TokenType::ABSTRACT_DECLARATOR);

    if (matchToken(TokenType::MUL))
    {
        node->children.push_back(pointer());

        if (matchToken(TokenType::L_BR) || matchToken(TokenType::L_SQR))
        {
            node->children.push_back(directAbstractDeclarator());
        }
    }
    else if (matchToken(TokenType::L_BR) || matchToken(TokenType::L_SQR))
    {
        node->children.push_back(directAbstractDeclarator());
    }

    return node;
}

std::shared_ptr<Node> AST::directAbstractDeclarator()
{
    auto node = std::make_shared<Node>(TokenType::DIRECT_ABSTRACT_DECLARATOR);

    if (matchToken(TokenType::L_BR))
    {
        auto lbr = getNextToken();
        node->children.push_back(std::make_shared<Node>(*lbr));

        if (matchToken(TokenType::R_BR))
        {
            auto rbr = getNextToken();
            node->children.push_back(std::make_shared<Node>(*rbr));
        }
        else
        {
            node->children.push_back(abstractDeclarator());
            expectToken(TokenType::R_BR, "Expected ')'");
            auto rbr = getNextToken();
            node->children.push_back(std::make_shared<Node>(*rbr));
        }
    }
    else if (matchToken(TokenType::L_SQR))
    {
        auto lsqr = getNextToken();
        node->children.push_back(std::make_shared<Node>(*lsqr));

        if (matchToken(TokenType::R_SQR))
        {
            auto rsqr = getNextToken();
            node->children.push_back(std::make_shared<Node>(*rsqr));
        }
        else if (matchToken(TokenType::MUL))
        {
            auto mul = getNextToken();
            node->children.push_back(std::make_shared<Node>(*mul));
            expectToken(TokenType::R_SQR, "Expected ']'");
            auto rsqr = getNextToken();
            node->children.push_back(std::make_shared<Node>(*rsqr));
        }
        else
        {
            // Handle array size specifications
            if (matchToken(TokenType::STATIC))
            {
                auto stat = getNextToken();
                node->children.push_back(std::make_shared<Node>(*stat));

                if (typeQualifier(peekNextToken()))
                {
                    node->children.push_back(typeQualifierList());
                }

                node->children.push_back(assignmentExpression());
            }
            else if (typeQualifier(peekNextToken()))
            {
                node->children.push_back(typeQualifierList());

                if (matchToken(TokenType::STATIC))
                {
                    auto stat = getNextToken();
                    node->children.push_back(std::make_shared<Node>(*stat));
                    node->children.push_back(assignmentExpression());
                }
                else if (matchToken(TokenType::MUL))
                {
                    auto mul = getNextToken();
                    node->children.push_back(std::make_shared<Node>(*mul));
                }
                else
                {
                    node->children.push_back(assignmentExpression());
                }
            }
            else
            {
                node->children.push_back(assignmentExpression());
            }

            expectToken(TokenType::R_SQR, "Expected ']'");
            auto rsqr = getNextToken();
            node->children.push_back(std::make_shared<Node>(*rsqr));
        }
    }

    // Parse additional array/function suffixes
    while (true)
    {
        if (matchToken(TokenType::L_SQR))
        {
            auto lsqr = getNextToken();
            node->children.push_back(std::make_shared<Node>(*lsqr));

            if (matchToken(TokenType::R_SQR))
            {
                auto rsqr = getNextToken();
                node->children.push_back(std::make_shared<Node>(*rsqr));
            }
            else if (matchToken(TokenType::MUL))
            {
                auto mul = getNextToken();
                node->children.push_back(std::make_shared<Node>(*mul));
                expectToken(TokenType::R_SQR, "Expected ']'");
                auto rsqr = getNextToken();
                node->children.push_back(std::make_shared<Node>(*rsqr));
            }
            else
            {
                node->children.push_back(assignmentExpression());
                expectToken(TokenType::R_SQR, "Expected ']'");
                auto rsqr = getNextToken();
                node->children.push_back(std::make_shared<Node>(*rsqr));
            }
        }
        else if (matchToken(TokenType::L_BR))
        {
            auto lbr = getNextToken();
            node->children.push_back(std::make_shared<Node>(*lbr));

            if (matchToken(TokenType::R_BR))
            {
                auto rbr = getNextToken();
                node->children.push_back(std::make_shared<Node>(*rbr));
            }
            else
            {
                node->children.push_back(parameterTypeList());
                expectToken(TokenType::R_BR, "Expected ')'");
                auto rbr = getNextToken();
                node->children.push_back(std::make_shared<Node>(*rbr));
            }
        }
        else
        {
            break;
        }
    }

    return node;
}

std::shared_ptr<Node> AST::specifierQualifierList()
{
    auto node = std::make_shared<Node>(TokenType::SPECIFIER_QUALIFIER_LIST);

    while (typeSpecifier(peekNextToken()) || typeQualifier(peekNextToken()))
    {
        auto token = peekNextToken();

        if (typeSpecifier(token))
        {
            node->children.push_back(typeSpecifier());
        }
        else if (typeQualifier(token))
        {
            node->children.push_back(typeQualifier());
        }
    }

    return node;
}

// Struct and Union
std::shared_ptr<Node> AST::structOrUnionSpecifier()
{
    auto node = std::make_shared<Node>(TokenType::STRUCT_UNION_SPECIFIER);

    node->children.push_back(structOrUnion());

    if (matchToken(TokenType::ID))
    {
        auto id = getNextToken();
        node->children.push_back(std::make_shared<Node>(*id));
    }

    if (matchToken(TokenType::L_CUR))
    {
        auto lcur = getNextToken();
        node->children.push_back(std::make_shared<Node>(*lcur));
        node->children.push_back(structDeclarationList());
        expectToken(TokenType::R_CUR, "Expected '}' after struct declaration list");
        auto rcur = getNextToken();
        node->children.push_back(std::make_shared<Node>(*rcur));
    }

    return node;
}

std::shared_ptr<Node> AST::structOrUnion()
{
    auto token = getNextToken();
    return std::make_shared<Node>(*token);
}

std::shared_ptr<Node> AST::structDeclarationList()
{
    auto node = std::make_shared<Node>(TokenType::STRUCT_DECLARATION_LIST);

    while (!matchToken(TokenType::R_CUR) && !matchToken(TokenType::END))
    {
        node->children.push_back(structDeclaration());
    }

    return node;
}

std::shared_ptr<Node> AST::structDeclaration()
{
    auto node = std::make_shared<Node>(TokenType::STRUCT_DECLARATION);

    // Check for static_assert
    if (matchToken(TokenType::STATIC) && peekNextToken()->lexeme == "_" &&
        std::next(peekNextToken()) != symbolTable.end() &&
        std::next(peekNextToken())->lexeme == "Assert")
    {
        return staticAssertDeclaration();
    }

    node->children.push_back(specifierQualifierList());

    if (matchToken(TokenType::SEMI_COLON))
    {
        auto semi = getNextToken();
        node->children.push_back(std::make_shared<Node>(*semi));
        return node;
    }

    node->children.push_back(structDeclaratorList());
    expectToken(TokenType::SEMI_COLON, "Expected ';' after struct declarator");
    auto semi = getNextToken();
    node->children.push_back(std::make_shared<Node>(*semi));

    return node;
}

std::shared_ptr<Node> AST::structDeclaratorList()
{
    auto node = std::make_shared<Node>(TokenType::STRUCT_DECLARATOR_LIST);

    node->children.push_back(structDeclarator());

    while (matchToken(TokenType::COMMA))
    {
        auto comma = getNextToken();
        node->children.push_back(std::make_shared<Node>(*comma));
        node->children.push_back(structDeclarator());
    }

    return node;
}

std::shared_ptr<Node> AST::structDeclarator()
{
    auto node = std::make_shared<Node>(TokenType::STRUCT_DECLARATOR);

    if (matchToken(TokenType::COLON))
    {
        auto colon = getNextToken();
        node->children.push_back(std::make_shared<Node>(*colon));
        node->children.push_back(constantExpression());
    }
    else
    {
        node->children.push_back(declarator());

        if (matchToken(TokenType::COLON))
        {
            auto colon = getNextToken();
            node->children.push_back(std::make_shared<Node>(*colon));
            node->children.push_back(constantExpression());
        }
    }

    return node;
}

// Enum
std::shared_ptr<Node> AST::enumSpecifier()
{
    auto node = std::make_shared<Node>(TokenType::ENUM_SPECIFIER);

    expectToken(TokenType::ENUM, "Expected 'enum'");
    auto enumToken = getNextToken();
    node->children.push_back(std::make_shared<Node>(*enumToken));

    if (matchToken(TokenType::ID))
    {
        auto id = getNextToken();
        node->children.push_back(std::make_shared<Node>(*id));
    }

    if (matchToken(TokenType::L_CUR))
    {
        auto lcur = getNextToken();
        node->children.push_back(std::make_shared<Node>(*lcur));
        node->children.push_back(enumeratorList());

        if (matchToken(TokenType::COMMA))
        {
            auto comma = getNextToken();
            node->children.push_back(std::make_shared<Node>(*comma));
        }

        expectToken(TokenType::R_CUR, "Expected '}' after enumerator list");
        auto rcur = getNextToken();
        node->children.push_back(std::make_shared<Node>(*rcur));
    }

    return node;
}

std::shared_ptr<Node> AST::enumeratorList()
{
    auto node = std::make_shared<Node>(TokenType::ENUMERATOR_LIST);

    node->children.push_back(enumerator());

    while (matchToken(TokenType::COMMA))
    {
        auto comma = getNextToken();
        node->children.push_back(std::make_shared<Node>(*comma));

        if (matchToken(TokenType::ID))
        {
            node->children.push_back(enumerator());
        }
    }

    return node;
}

std::shared_ptr<Node> AST::enumerator()
{
    auto node = std::make_shared<Node>(TokenType::ENUMERATOR);

    expectToken(TokenType::ID, "Expected identifier in enumerator");
    auto id = getNextToken();
    node->children.push_back(std::make_shared<Node>(*id));

    if (matchToken(TokenType::ASSIGN))
    {
        auto assign = getNextToken();
        node->children.push_back(std::make_shared<Node>(*assign));
        node->children.push_back(constantExpression());
    }

    return node;
}

std::shared_ptr<Node> AST::enumerationConstant()
{
    if (matchToken(TokenType::ID))
    {
        auto token = getNextToken();
        return std::make_shared<Node>(*token);
    }

    loggedError.addError(lineNo, "Expected enumeration constant");
    return std::make_shared<Node>(TokenType::ID);
}

// Initializer
std::shared_ptr<Node> AST::initializer()
{
    auto node = std::make_shared<Node>(TokenType::INITIALIZER);

    if (matchToken(TokenType::L_CUR))
    {
        auto lcur = getNextToken();
        node->children.push_back(std::make_shared<Node>(*lcur));
        node->children.push_back(initializerList());

        if (matchToken(TokenType::COMMA))
        {
            auto comma = getNextToken();
            node->children.push_back(std::make_shared<Node>(*comma));
        }

        expectToken(TokenType::R_CUR, "Expected '}' after initializer list");
        auto rcur = getNextToken();
        node->children.push_back(std::make_shared<Node>(*rcur));
    }
    else
    {
        node->children.push_back(assignmentExpression());
    }

    return node;
}

std::shared_ptr<Node> AST::initializerList()
{
    auto node = std::make_shared<Node>(TokenType::INITIALIZER);

    if (matchToken(TokenType::L_SQR) || matchToken(TokenType::DOT))
    {
        node->children.push_back(designation());
    }

    node->children.push_back(initializer());

    while (matchToken(TokenType::COMMA))
    {
        auto comma = getNextToken();
        node->children.push_back(std::make_shared<Node>(*comma));

        if (matchToken(TokenType::L_SQR) || matchToken(TokenType::DOT))
        {
            node->children.push_back(designation());
        }

        node->children.push_back(initializer());
    }

    return node;
}

std::shared_ptr<Node> AST::designation()
{
    auto node = std::make_shared<Node>(TokenType::ASSIGNMENT_EXP);

    node->children.push_back(designatorList());
    expectToken(TokenType::ASSIGN, "Expected '=' after designator");
    auto assign = getNextToken();
    node->children.push_back(std::make_shared<Node>(*assign));

    return node;
}

std::shared_ptr<Node> AST::designatorList()
{
    auto node = std::make_shared<Node>(TokenType::ASSIGNMENT_EXP);

    node->children.push_back(designator());

    while (matchToken(TokenType::L_SQR) || matchToken(TokenType::DOT))
    {
        node->children.push_back(designator());
    }

    return node;
}

std::shared_ptr<Node> AST::designator()
{
    auto node = std::make_shared<Node>(TokenType::ASSIGNMENT_EXP);

    if (matchToken(TokenType::L_SQR))
    {
        auto lsqr = getNextToken();
        node->children.push_back(std::make_shared<Node>(*lsqr));
        node->children.push_back(constantExpression());
        expectToken(TokenType::R_SQR, "Expected ']'");
        auto rsqr = getNextToken();
        node->children.push_back(std::make_shared<Node>(*rsqr));
    }
    else if (matchToken(TokenType::DOT))
    {
        auto dot = getNextToken();
        node->children.push_back(std::make_shared<Node>(*dot));
        expectToken(TokenType::ID, "Expected identifier after '.'");
        auto id = getNextToken();
        node->children.push_back(std::make_shared<Node>(*id));
    }

    return node;
}

// Statement parsing
std::shared_ptr<Node> AST::statement()
{
    auto token = peekNextToken();

    if (token->type == TokenType::ID && std::next(token) != symbolTable.end() &&
        std::next(token)->type == TokenType::COLON)
    {
        return labeledStatement();
    }
    else if (token->type == TokenType::CASE)
    {
        return labeledStatement();
    }
    else if (token->type == TokenType::DEFAULT)
    {
        return labeledStatement();
    }
    else if (token->type == TokenType::L_CUR)
    {
        return compoundStatement();
    }
    else if (token->type == TokenType::SEMI_COLON)
    {
        return expressionStatement();
    }
    else if (token->type == TokenType::IF || token->type == TokenType::SWITCH)
    {
        return selectionStatement();
    }
    else if (token->type == TokenType::WHILE || token->type == TokenType::DO || token->type == TokenType::FOR)
    {
        return iterationStatement();
    }
    else if ((token->lexeme == "goto") || token->type == TokenType::CONT ||
             token->type == TokenType::BRK || token->type == TokenType::RETURN)
    {
        return jumpStatement();
    }
    else
    {
        return expressionStatement();
    }
}

std::shared_ptr<Node> AST::labeledStatement()
{
    auto node = std::make_shared<Node>(TokenType::ASSIGNMENT_EXP);

    auto token = peekNextToken();

    if (token->type == TokenType::ID)
    {
        auto id = getNextToken();
        node->children.push_back(std::make_shared<Node>(*id));
        expectToken(TokenType::COLON, "Expected ':' after label");
        auto colon = getNextToken();
        node->children.push_back(std::make_shared<Node>(*colon));
    }
    else if (token->type == TokenType::CASE)
    {
        auto caseToken = getNextToken();
        node->children.push_back(std::make_shared<Node>(*caseToken));
        node->children.push_back(constantExpression());
        expectToken(TokenType::COLON, "Expected ':' after case");
        auto colon = getNextToken();
        node->children.push_back(std::make_shared<Node>(*colon));
    }
    else if (token->type == TokenType::DEFAULT)
    {
        auto defToken = getNextToken();
        node->children.push_back(std::make_shared<Node>(*defToken));
        expectToken(TokenType::COLON, "Expected ':' after default");
        auto colon = getNextToken();
        node->children.push_back(std::make_shared<Node>(*colon));
    }

    node->children.push_back(statement());

    return node;
}

std::shared_ptr<Node> AST::compoundStatement()
{
    auto node = std::make_shared<Node>(TokenType::ASSIGNMENT_EXP);

    expectToken(TokenType::L_CUR, "Expected '{'");
    auto lcur = getNextToken();
    node->children.push_back(std::make_shared<Node>(*lcur));

    if (!matchToken(TokenType::R_CUR))
    {
        node->children.push_back(blockItemList());
    }

    expectToken(TokenType::R_CUR, "Expected '}'");
    auto rcur = getNextToken();
    node->children.push_back(std::make_shared<Node>(*rcur));

    return node;
}

std::shared_ptr<Node> AST::blockItemList()
{
    auto node = std::make_shared<Node>(TokenType::DECLARATION_SPECIFIERS);

    node->children.push_back(blockItem());

    while (!matchToken(TokenType::R_CUR) && !matchToken(TokenType::END))
    {
        node->children.push_back(blockItem());
    }

    return node;
}

std::shared_ptr<Node> AST::blockItem()
{
    auto token = peekNextToken();

    if (storageClassSpecifier(token) || typeSpecifier(token) || typeQualifier(token) ||
        isTypeName(token->lexeme))
    {
        return declaration();
    }
    else
    {
        return statement();
    }
}

std::shared_ptr<Node> AST::expressionStatement()
{
    auto node = std::make_shared<Node>(TokenType::ASSIGNMENT_EXP);

    if (matchToken(TokenType::SEMI_COLON))
    {
        auto semi = getNextToken();
        node->children.push_back(std::make_shared<Node>(*semi));
        return node;
    }

    node->children.push_back(expression());
    expectToken(TokenType::SEMI_COLON, "Expected ';' after expression");
    auto semi = getNextToken();
    node->children.push_back(std::make_shared<Node>(*semi));

    return node;
}

std::shared_ptr<Node> AST::selectionStatement()
{
    auto node = std::make_shared<Node>(TokenType::ASSIGNMENT_EXP);

    auto token = peekNextToken();

    if (token->type == TokenType::IF)
    {
        auto ifToken = getNextToken();
        node->children.push_back(std::make_shared<Node>(*ifToken));
        expectToken(TokenType::L_BR, "Expected '(' after 'if'");
        auto lbr = getNextToken();
        node->children.push_back(std::make_shared<Node>(*lbr));
        node->children.push_back(expression());
        expectToken(TokenType::R_BR, "Expected ')' after expression");
        auto rbr = getNextToken();
        node->children.push_back(std::make_shared<Node>(*rbr));
        node->children.push_back(statement());

        if (matchToken(TokenType::ELSE))
        {
            auto elseToken = getNextToken();
            node->children.push_back(std::make_shared<Node>(*elseToken));
            node->children.push_back(statement());
        }
    }
    else if (token->type == TokenType::SWITCH)
    {
        auto switchToken = getNextToken();
        node->children.push_back(std::make_shared<Node>(*switchToken));
        expectToken(TokenType::L_BR, "Expected '(' after 'switch'");
        auto lbr = getNextToken();
        node->children.push_back(std::make_shared<Node>(*lbr));
        node->children.push_back(expression());
        expectToken(TokenType::R_BR, "Expected ')' after expression");
        auto rbr = getNextToken();
        node->children.push_back(std::make_shared<Node>(*rbr));
        node->children.push_back(statement());
    }

    return node;
}

std::shared_ptr<Node> AST::iterationStatement()
{
    auto node = std::make_shared<Node>(TokenType::ASSIGNMENT_EXP);

    auto token = peekNextToken();

    if (token->type == TokenType::WHILE)
    {
        auto whileToken = getNextToken();
        node->children.push_back(std::make_shared<Node>(*whileToken));
        expectToken(TokenType::L_BR, "Expected '(' after 'while'");
        auto lbr = getNextToken();
        node->children.push_back(std::make_shared<Node>(*lbr));
        node->children.push_back(expression());
        expectToken(TokenType::R_BR, "Expected ')' after expression");
        auto rbr = getNextToken();
        node->children.push_back(std::make_shared<Node>(*rbr));
        node->children.push_back(statement());
    }
    else if (token->type == TokenType::DO)
    {
        auto doToken = getNextToken();
        node->children.push_back(std::make_shared<Node>(*doToken));
        node->children.push_back(statement());
        expectToken(TokenType::WHILE, "Expected 'while' after 'do'");
        auto whileToken = getNextToken();
        node->children.push_back(std::make_shared<Node>(*whileToken));
        expectToken(TokenType::L_BR, "Expected '(' after 'while'");
        auto lbr = getNextToken();
        node->children.push_back(std::make_shared<Node>(*lbr));
        node->children.push_back(expression());
        expectToken(TokenType::R_BR, "Expected ')' after expression");
        auto rbr = getNextToken();
        node->children.push_back(std::make_shared<Node>(*rbr));
        expectToken(TokenType::SEMI_COLON, "Expected ';' after do-while");
        auto semi = getNextToken();
        node->children.push_back(std::make_shared<Node>(*semi));
    }
    else if (token->type == TokenType::FOR)
    {
        auto forToken = getNextToken();
        node->children.push_back(std::make_shared<Node>(*forToken));
        expectToken(TokenType::L_BR, "Expected '(' after 'for'");
        auto lbr = getNextToken();
        node->children.push_back(std::make_shared<Node>(*lbr));

        // Parse for loop initialization
        if (storageClassSpecifier(peekNextToken()) || typeSpecifier(peekNextToken()) ||
            typeQualifier(peekNextToken()) || isTypeName(peekNextToken()->lexeme))
        {
            node->children.push_back(declaration());
        }
        else
        {
            node->children.push_back(expressionStatement());
        }

        // Parse condition
        node->children.push_back(expressionStatement());

        // Parse increment (optional)
        if (!matchToken(TokenType::R_BR))
        {
            node->children.push_back(expression());
        }

        expectToken(TokenType::R_BR, "Expected ')' after for loop");
        auto rbr = getNextToken();
        node->children.push_back(std::make_shared<Node>(*rbr));
        node->children.push_back(statement());
    }

    return node;
}

std::shared_ptr<Node> AST::jumpStatement()
{
    auto node = std::make_shared<Node>(TokenType::ASSIGNMENT_EXP);

    auto token = peekNextToken();

    // Note: GOTO token type not available, checking by lexeme
    if (token != symbolTable.end() && token->lexeme == "goto")
    {
        auto gotoToken = getNextToken();
        node->children.push_back(std::make_shared<Node>(*gotoToken));
        expectToken(TokenType::ID, "Expected identifier after 'goto'");
        auto id = getNextToken();
        node->children.push_back(std::make_shared<Node>(*id));
        expectToken(TokenType::SEMI_COLON, "Expected ';'");
        auto semi = getNextToken();
        node->children.push_back(std::make_shared<Node>(*semi));
    }
    else if (token->type == TokenType::CONT || token->type == TokenType::BRK)
    {
        auto token = getNextToken();
        node->children.push_back(std::make_shared<Node>(*token));
        expectToken(TokenType::SEMI_COLON, "Expected ';'");
        auto semi = getNextToken();
        node->children.push_back(std::make_shared<Node>(*semi));
    }
    else if (token->type == TokenType::RETURN)
    {
        auto returnToken = getNextToken();
        node->children.push_back(std::make_shared<Node>(*returnToken));

        if (!matchToken(TokenType::SEMI_COLON))
        {
            node->children.push_back(expression());
        }

        expectToken(TokenType::SEMI_COLON, "Expected ';' after return");
        auto semi = getNextToken();
        node->children.push_back(std::make_shared<Node>(*semi));
    }

    return node;
}

std::shared_ptr<Node> AST::staticAssertDeclaration()
{
    auto node = std::make_shared<Node>(TokenType::DECLARATION_SPECIFIERS);

    // STATIC_ASSERT would need to be added to scanner
    // For now, basic implementation
    expectToken(TokenType::STATIC, "Expected 'static'");
    auto stat = getNextToken();
    node->children.push_back(std::make_shared<Node>(*stat));

    expectToken(TokenType::L_BR, "Expected '('");
    auto lbr = getNextToken();
    node->children.push_back(std::make_shared<Node>(*lbr));

    node->children.push_back(constantExpression());

    expectToken(TokenType::COMMA, "Expected ','");
    auto comma = getNextToken();
    node->children.push_back(std::make_shared<Node>(*comma));

    expectToken(TokenType::STRING_LITERAL, "Expected string literal");
    auto str = getNextToken();
    node->children.push_back(std::make_shared<Node>(*str));

    expectToken(TokenType::R_BR, "Expected ')'");
    auto rbr = getNextToken();
    node->children.push_back(std::make_shared<Node>(*rbr));

    expectToken(TokenType::SEMI_COLON, "Expected ';'");
    auto semi = getNextToken();
    node->children.push_back(std::make_shared<Node>(*semi));

    return node;
}

// Include statement (existing implementation)
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

    if ((itr->type == TokenType::DOUBLE_QUOTE && tempType == TokenType::DOUBLE_QUOTE) ||
        (itr->type == TokenType::GT && tempType == TokenType::LT))
        ret->children.push_back(std::make_shared<Node>(std::move(*itr)));
    else
    {
        loggedError.addError(lineNo, INCLUD_ERROR);
        return ret;
    }
    return ret;
}

// Print functions (existing implementation)
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
