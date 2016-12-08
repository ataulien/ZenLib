/*
 * Copyright (C) 2016 Hedede <haddayn@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 */
#include <cassert>
#include <vector>

#include <daedalus/compiler/Declaration.h>
#include <daedalus/compiler/Statement.h>
#include <daedalus/compiler/Expression.h>

#include <daedalus/compiler/SymbolTable.h>

#include <daedalus/compiler/PrintToken.h>
#include <daedalus/compiler/DiagnosticHelper.h>
#include <daedalus/compiler/Parser.h>

namespace Daedalus {
bool isTypeName(Token tok)
{
	return isIdentifier(tok)       ||
	       tok == Token::kw_void   ||
	       tok == Token::kw_float  ||
	       tok == Token::kw_int    ||
	       tok == Token::kw_string ||
	       tok == Token::kw_func;
}

template<typename...Args>
std::nullptr_t error(DiagnosticHelper& diag, Token tok,
                     Diagnostic::ID id, Args... args)
{
	Diagnostic msg(tok.location(), id);

	msg << tok.data();
	using fold_dummy = int[];
	(void) fold_dummy { 0, (void(msg << args), 0)...  };
	diag.report(msg);

	return nullptr;
}

std::nullptr_t
error_unexpected_token(DiagnosticHelper& diag, Token tok, Token::Kind expected)
{
	return error(diag, tok, Diagnostic::UnexpectedToken, expected);
}

std::nullptr_t
error_expected_constant_initializer(DiagnosticHelper& diag, Token tok)
{
	return error(diag, tok, Diagnostic::ExpectedConstantInitializer);
}

std::nullptr_t
error_expected_instance_name(DiagnosticHelper& diag, Token tok)
{
	return error(diag, tok, Diagnostic::ExpectedInstanceName);
}

std::nullptr_t
error_expected_prototype_name(DiagnosticHelper& diag, Token tok)
{
	return error(diag, tok, Diagnostic::ExpectedPrototypeName);
}

std::nullptr_t
error_expected_semicolon_after_expr(DiagnosticHelper& diag, Token tok)
{
	return error(diag, tok, Diagnostic::ExpectedSemicolonAfterExpr);
}

std::nullptr_t
error_expected_semicolon_after_var(DiagnosticHelper& diag, Token tok)
{
	return error(diag, tok, Diagnostic::ExpectedSemicolonAfterVar);
}

std::nullptr_t
error_expected_semicolon_after_do_while(DiagnosticHelper& diag, Token tok)
{
	return error(diag, tok, Diagnostic::ExpectedSemicolonAfterDoWhile);
}

bool Parser::match(Token::Kind expected)
{
	if (token.type() != expected)
		return false;

	// consume token
	getNextToken();
	return true;
}

std::unique_ptr<tree::Declaration>
Parser::parseDeclaration()
{
	std::unique_ptr<tree::Declaration> decl;

	switch(token.type()) {
	case Token::kw_var:
		decl = parseGlobalVar();
		break;
	case Token::kw_const:
		decl = parseConstant();
		break;
	case Token::kw_func:
		decl = parseFunctionDefinition();
		break;
	case Token::kw_class:
		decl = parseClass();
		break;
	case Token::kw_prototype:
		decl = parsePrototype();
		break;
	case Token::kw_instance:
		decl = parseInstance();
		break;
	case Token::eof:
		return nullptr;
	default:
		return error(diag, token, Diagnostic::UnexpectedToken, "declaration");
	}

	/* TODO: do not forget about global variables*/
	while (match(Token::semicolon));

	return decl;
}

Class* Parser::readType()
{
	if (!isTypeName(token))
		return error(diag, token, Diagnostic::UnexpectedToken, "type name");

	auto ref = symtab.getSymbol(token.data());
	if (ref.kind != Symbol::Class)
		return error(diag, token, Diagnostic::UnknownType);

	auto scope = symtab.getScope(ref);
	if (!scope)
		return error(diag, token, Diagnostic::UnknownType);

	//return symtab.find(token);
	auto& type = scope->classes[ref.index];

	getNextToken(); // consume identifier

	return &type;
}

/*
 * variableDecl ::= 'var' type id
 */
std::unique_ptr<tree::Variable>
Parser::parseVariable(Type type)
{
	// Read variable name
	if (!isIdentifier(token))
		return error(diag, token, Diagnostic::ExpectedIdentifier);
	
	auto name = token.data();

	getNextToken(); // consume identifier

	auto var = std::make_unique<tree::Variable>(name, type);

	if (match(Token::l_bracket)) {
		auto size_expr = parseExpression();
		if (!size_expr)
			return nullptr;

		var->setSizeExpr(std::move(size_expr));

		if (!match(Token::r_bracket))
			return error_unexpected_token(diag, token, Token::r_brace);
	}

	return var;
}

std::unique_ptr<tree::Declaration>
Parser::parseGlobalVar()
{
	getNextToken(); // consume 'var';

	auto type = readType();
	if (!type)
		return nullptr;

	Type var_type{type, false, 1};

	auto var = parseVariable(var_type);
	if (!var)
		return nullptr;

	if (match(Token::semicolon))
		return var;

	return error_expected_semicolon_after_var(diag, token);
}

std::unique_ptr<tree::Declaration>
Parser::parseLocalVar()
{
	getNextToken(); // consume 'var';

	auto type = readType();
	if (!type)
		return nullptr;

	Type var_type{type, false, 1};

	auto var = parseVariable(var_type);
	if (!var)
		return nullptr;

	// allow initializer for non-array vars
	if (!var->isArray() && match(Token::equal)) {
		auto initializer = parseExpression();
		if (!initializer)
			return nullptr;
		var->setInitialier(std::move(initializer));
	}

	if (match(Token::semicolon))
		return var;

	return error_expected_semicolon_after_var(diag, token);
}

/*
 * constantDecl ::= 'const' id id '=' expr
 */
std::unique_ptr<tree::Declaration>
Parser::parseConstant()
{
	getNextToken(); // consume 'const';

	auto type = readType();
	if (!type)
		return nullptr;

	Type var_type{type, true, 1};

	auto var = parseVariable(var_type);
	if (!var)
		return nullptr;

	// Read constant initializer
	if (!match(Token::equal))
		return error_expected_constant_initializer(diag, token);

	std::unique_ptr<tree::Expression> initializer;
	if (match(Token::l_brace))
		initializer = parseArrayInitializer();
	else
		initializer = parseExpression();

	if (!initializer)
		return nullptr;

	var->setInitialier(std::move(initializer));

	if (match(Token::semicolon))
		return var;

	return error_expected_semicolon_after_var(diag, token);
}

std::unique_ptr<tree::Expression>
Parser::parseArrayInitializer()
{
	std::vector<std::unique_ptr<tree::Expression>> initList;
	while (!match(Token::r_brace)) {
		auto expr = parseExpression();
		if (!expr)
			return nullptr;
		initList.push_back(std::move(expr));

		if (token == Token::r_brace)
			break;

		if (!match(Token::comma))
			return error_unexpected_token(diag, token, Token::comma);
	}

	if (!match(Token::r_brace))
		return error(diag, token, Diagnostic::ExpectedExpression);

	return std::make_unique<tree::ArrayInitializer>(std::move(initList));
}

/*
 * functionDecl ::= 'func' id id '(' args ')'
 *         args ::= arg (',' args)?
 *          arg ::= variableDecl
 */
std::unique_ptr<tree::FunctionProto>
Parser::parseFunctionPrototype()
{
	auto type = readType();
	if (!type)
		return nullptr;

	Type ret{type, false, 1};

	// Function name
	if (!isIdentifier(token))
		return error(diag, token, Diagnostic::ExpectedIdentifier);

	std::string name = token.data();

	// consume identifier
	getNextToken();
	if (!match(Token::l_paren))
		return error_unexpected_token(diag, token, Token::l_paren);

	// Argument list
	std::vector<std::unique_ptr<tree::Variable>> args;

	while (match(Token::kw_var)) {
		auto type = readType();
		if (!type)
			return nullptr;

		Type arg_type{type, false, 1};
		auto arg = parseVariable(arg_type);
		if (!arg)
			return nullptr;
		if (arg->isArray())
			return error(diag, token, Diagnostic::ArrayNotAllowed);

		args.push_back(std::move(arg));

		if (token.type() == Token::r_paren)
			break;

		if (!match(Token::comma))
			return error_unexpected_token(diag, token, Token::comma);
	}

	if (!match(Token::r_paren))
		return error(diag, token, Diagnostic::ExpectedVariableDecl);

	return std::make_unique<tree::FunctionProto>(name, ret, std::move(args));
}

/*
 * functionDef  ::= functionDecl '{' stmts '}'
 */
std::unique_ptr<tree::Declaration>
Parser::parseFunctionDefinition()
{
	getNextToken(); // consume 'func'

	auto proto = parseFunctionPrototype();

	if (!proto)
		return nullptr;

	if (token.type() == Token::semicolon)
		return proto;

	auto body = parseStatementBlock();

	if (!body)
		return nullptr;

	return std::make_unique<tree::Function>(std::move(proto), std::move(body));
}

/*
 *      classDecl ::= 'class' id
 *          class ::= classDecl '{' classBodyDecls '}'
 * classBodyDecl  ::= functionDecl | varDecl
 * classBodyDecls ::= classBodyDecl ';' classBodyDecls?
 */
std::unique_ptr<tree::Declaration>
Parser::parseClass()
{
	// Class name
	if (!isIdentifier(getNextToken()))
		return error(diag, token, Diagnostic::ExpectedIdentifier);

	std::string name = token.data();

	getNextToken(); // consume name;

	if (!match(Token::l_brace))
		return error_unexpected_token(diag, token, Token::l_brace);

	// Class members
	std::vector<std::unique_ptr<tree::Variable>> members;
	while (match(Token::kw_var)) {
		auto type = readType();
		if (!type)
			return nullptr;

		Type var_type{type, false, 1};
		auto var = parseVariable(var_type);
		if (var == nullptr)
			return nullptr;

		members.push_back(std::move(var));

		if (token == Token::r_brace)
			break;

		while (match(Token::comma)) {
			auto var = parseVariable(var_type);
			if (var == nullptr)
				return nullptr;

			members.push_back(std::move(var));
		}

		if (!match(Token::semicolon))
			return error_expected_semicolon_after_var(diag, token);
	}

	if (!match(Token::r_brace))
		return error_unexpected_token(diag, token, Token::r_brace);

	return std::make_unique<tree::Class>(name, std::move(members));
}

std::unique_ptr<tree::Prototype>
Parser::parsePrototype()
{
	if (!isIdentifier(getNextToken()))
		return error_expected_prototype_name(diag, token);

	std::string name = token.data();
	getNextToken(); // consume name;

	if (!match(Token::l_paren))
		return error_unexpected_token(diag, token, Token::l_paren);

	std::string base = token.data();
	getNextToken();

	if (!match(Token::r_paren))
		return error_unexpected_token(diag, token, Token::r_paren);

	auto body = parseStatementBlock();
	if (!body)
		return nullptr;

	return std::make_unique<tree::Prototype>(name, base, std::move(body));
}

std::unique_ptr<tree::Instance>
Parser::parseInstance()
{
	if (!isIdentifier(getNextToken()))
		return error_expected_instance_name(diag, token);

	StringList names;

	std::string name = token.data();
	getNextToken(); // consume name;

	while (match(Token::comma)) {
		names.push_back(name);
		name = token.data();
		getNextToken();
	}

	names.push_back(name);

	if (!match(Token::l_paren))
		return error_unexpected_token(diag, token, Token::l_paren);

	std::string base = token.data();
	getNextToken();

	if (!match(Token::r_paren))
		return error_unexpected_token(diag, token, Token::r_paren);

	std::unique_ptr<tree::StatementBlock> body = nullptr;

	if (!match(Token::semicolon)) {
		auto body = parseStatementBlock();
		if (!body)
			return nullptr;
	}

	return std::make_unique<tree::Instance>(std::move(names), base, std::move(body));
}

std::unique_ptr<tree::Statement>
Parser::parseStatement()
{
	switch (token.type()) {
	case Token::kw_if:
		getNextToken();
		return parseBranchStatement();
	case Token::kw_return:
		getNextToken();
		return parseReturnStatement();
	case Token::kw_while:
		getNextToken();
		return parseWhileStatement();
	case Token::kw_do:
		getNextToken();
		return parseDoStatement();
	case Token::kw_break:
	case Token::kw_continue:
		return parseBreakStatement();
	case Token::kw_const:
	case Token::kw_var:
		return parseLocal();
	case Token::l_brace:
		return parseStatementBlock();
	default:
		return parseExprStatement();
	}
}

std::unique_ptr<tree::Statement>
Parser::parseBreakStatement()
{
	assert(token == Token::kw_break || token == Token::kw_continue);

	std::unique_ptr<tree::Statement> stmt;
	if (token == Token::kw_break)
		stmt = std::make_unique<tree::BreakStatement>();
	else if (token == Token::kw_continue)
		stmt = std::make_unique<tree::ContinueStatement>();

	getNextToken();

	if (match(Token::semicolon))
		return stmt;

	return error_unexpected_token(diag, token, Token::semicolon);
}


std::unique_ptr<tree::Statement>
Parser::parseLocal()
{
	std::unique_ptr<tree::Declaration> var;
	if (token == Token::kw_var)
		var = parseLocalVar();

	if (token == Token::kw_const)
		var = parseConstant();

	if (var)
		return std::make_unique<tree::DeclStatement>(std::move(var));

	return nullptr;
}

std::unique_ptr<tree::Statement>
Parser::parseExprStatement()
{
	auto expr = parseExpression();

	if (!match(Token::semicolon))
		return error_expected_semicolon_after_expr(diag, token);

	return std::make_unique<tree::ExprStatement>(std::move(expr));
}

std::unique_ptr<tree::StatementBlock>
Parser::parseStatementBlock()
{
	if (!match(Token::l_brace))
		return error_unexpected_token(diag, token, Token::l_brace);

	std::vector<std::unique_ptr<tree::Statement>> statements;
	while (!match(Token::r_brace)) {
		auto statement = parseStatement();
		if (!statement)
			return nullptr;

		statements.push_back(std::move(statement));
	}

	while(match(Token::semicolon));

	return std::make_unique<tree::StatementBlock>(std::move(statements));
}

constexpr bool AllowParenlessIf = true;
std::unique_ptr<tree::Statement>
Parser::parseBranchStatement()
{
	std::unique_ptr<tree::Expression> ifExpr = AllowParenlessIf ?
	                parseExpression() : parseParenExpr();
	if (!ifExpr)
		return nullptr;

	std::unique_ptr<tree::Statement> ifBody = parseStatement();
	if (!ifBody)
		return nullptr;

	std::unique_ptr<tree::Statement> elseBody = nullptr;

	if (match(Token::kw_else)) {
		elseBody = parseStatement();

		if (!elseBody)
			return nullptr;
	}

	return std::make_unique<tree::IfStatement>(
	       std::move(ifExpr), std::move(ifBody), std::move(elseBody));
}

std::unique_ptr<tree::Statement>
Parser::parseWhileStatement()
{
	std::unique_ptr<tree::Expression> ifExpr = AllowParenlessIf ?
	                parseExpression() : parseParenExpr();
	if (!ifExpr)
		return nullptr;

	std::unique_ptr<tree::Statement> body = parseStatement();
	if (!body)
		return nullptr;

	return std::make_unique<tree::WhileStatement>(
	                std::move(ifExpr), std::move(body));
}

std::unique_ptr<tree::Statement>
Parser::parseDoStatement()
{
	std::unique_ptr<tree::Statement> body = parseStatement();
	if (!body)
		return nullptr;

	if (!match(Token::kw_while))
		return error_unexpected_token(diag, token, Token::kw_while);

	std::unique_ptr<tree::Expression> ifExpr = AllowParenlessIf ?
	                parseExpression() : parseParenExpr();
	if (!ifExpr)
		return nullptr;

	if (!match(Token::semicolon))
		return error_expected_semicolon_after_do_while(diag, token);

	return std::make_unique<tree::DoStatement>(
	                std::move(ifExpr), std::move(body));
}


std::unique_ptr<tree::Statement>
Parser::parseReturnStatement()
{
	if (match(Token::semicolon))
		return std::make_unique<tree::ReturnStatement>(nullptr);

	std::unique_ptr<tree::Expression> retExpr = parseExpression();
	if (!retExpr)
		return nullptr;

	if (!match(Token::semicolon))
		return error_expected_semicolon_after_expr(diag, token);

	return std::make_unique<tree::ReturnStatement>(
	        std::move(retExpr));
}


/********************** Expressions **********************/
std::unique_ptr<tree::Expression>
Parser::parseExpression()
{
	// Parse left hand side
	std::unique_ptr<tree::Expression> lhs = parseUnaryExpr();

	if (!lhs)
		return nullptr;

	return parseBinaryExpr(std::move(lhs), prec::Unknown);
}

std::unique_ptr<tree::Expression>
Parser::parsePrimaryExpr()
{
	switch (token.type()) {
	case Token::l_paren:
		return parseParenExpr();
	case Token::identifier:
		return parseIdentifierExpr();
	case Token::numeric_constant:
		return parseNumberExpr();
	case Token::string_literal:
		return parseStringExpr();
	default:
		return error(diag, token, Diagnostic::ExpectedExpression);
	}
}

std::unique_ptr<tree::Expression>
Parser::parseParenExpr()
{
	if (!match(Token::l_paren))
		return error_unexpected_token(diag, token, Token::l_paren);

	std::unique_ptr<tree::Expression> expr = parseExpression();

	if (!match(Token::r_paren))
		return error_unexpected_token(diag, token, Token::r_paren);

	return expr;
}

std::unique_ptr<tree::Expression>
Parser::parseBinaryExpr(std::unique_ptr<tree::Expression> LHS, prec::Level minPrec)
{
	while (1) {
		prec::Level curPrec = getOperatorPrecedence(token);
		if (curPrec < minPrec)
			return LHS;

		Token opcode = token;
		getNextToken(); // consume operator

		std::unique_ptr<tree::Expression> RHS = parseUnaryExpr();
		if (!RHS)
			return nullptr;


		prec::Level nextPrec = getOperatorPrecedence(token);
		bool isRightAssoc = isRightAssociative(token);

		if (curPrec < nextPrec ||
		   (curPrec == nextPrec && isRightAssoc)) {
			RHS = parseBinaryExpr(
			       std::move(RHS),
			       prec::Level(curPrec + !isRightAssoc));
			if (!RHS)
				return nullptr;
		}

		LHS = std::make_unique<tree::BinaryExpr>(
		       opcode.type(), std::move(LHS), std::move(RHS));
	}
}

std::unique_ptr<tree::Expression>
Parser::parseUnaryExpr()
{
	if (!isUnaryOperator(token))
		return parsePrimaryExpr();

	Token opcode = token;
	getNextToken(); // consume operator

	std::unique_ptr<tree::Expression> operand = parseUnaryExpr();
	if (!operand)
		return nullptr;

	return std::make_unique<tree::UnaryExpr>(
	        opcode.type(), std::move(operand));
}

std::unique_ptr<tree::Expression>
Parser::parseIdentifierExpr()
{
	std::string name = token.data();

	getNextToken(); // consume identifier
	if (match(Token::l_paren))
		return parseCallExpr(name);

	std::unique_ptr<tree::Expression> id_expr;
	if (match(Token::dot)) {
		id_expr = parseFieldAccess(name);
	} else { 
		id_expr = std::make_unique<tree::IdentifierExpr>(name);
	}

	if (match(Token::l_bracket))
		id_expr = parseArraySubscript(std::move(id_expr));

	// TODO: postfix operators

	return id_expr;
}

std::unique_ptr<tree::Expression>
Parser::parseCallExpr(std::string func)
{
	std::vector<std::unique_ptr<tree::Expression>> args;

	if (!match(Token::r_paren)) {
		while (true) {
			auto arg = parseExpression();

			if (!arg)
				return nullptr;

			args.push_back(std::move(arg));

			if (match(Token::r_paren))
				break;

			if (!match(Token::comma))
				return error_unexpected_token(diag, token, Token::comma);
		}
	}

	return std::make_unique<tree::CallExpr>(
	        std::move(func), std::move(args));
}

std::unique_ptr<tree::Expression>
Parser::parseArraySubscript(std::unique_ptr<tree::Expression> array)
{
	auto arg = parseExpression();
	if (!arg)
		return nullptr;

	if (!match(Token::r_bracket))
		return error_unexpected_token(diag, token, Token::r_bracket);

	return std::make_unique<tree::SubscriptExpr>(
	                std::move(array), std::move(arg));
}

std::unique_ptr<tree::Expression>
Parser::parseFieldAccess(std::string id)
{
	if (!isIdentifier(token))
		return error(diag, token, Diagnostic::ExpectedIdentifier);

	std::string field = token.data();

	getNextToken(); // consume identifier

	return std::make_unique<tree::FieldExpr>(id, field);
}

std::unique_ptr<tree::Expression>
Parser::parseStringExpr()
{
	assert(token == Token::string_literal);

	Token tok = token;
	// Consume string
	getNextToken();

	return std::make_unique<tree::StringExpr>(tok.data());
}

std::unique_ptr<tree::Expression>
Parser::parseNumberExpr()
{
	assert(token == Token::numeric_constant);

	// store token, because we need to consume it
	Token tok = token;
	// consume number
	getNextToken();

	return std::make_unique<tree::NumberExpr>(tok.data());
}
} // namespace hrscript
