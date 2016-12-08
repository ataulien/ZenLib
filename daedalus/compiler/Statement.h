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
#ifndef Daedalus_Statement
#define Daedalus_Statement
#include <memory>
namespace Daedalus {
namespace tree {
class Statement {
public:
	virtual ~Statement() = default;

	//! List of concrete derived types
	enum Kind {
		StatementBlock,
		DeclStatement,
		IfStatement,
		WhileStatement,
		DoStatement,
		BreakStatement,
		ContinueStatement,
		ReturnStatement,
		ExprStatement,
	};

	Kind kind() const
	{
		return kind_;
	}

protected:
	Statement(Kind k)
		: kind_(k)
	{ }

private:
	Kind const kind_;
};

class Expression;

struct ExprStatement : Statement {
	ExprStatement(std::unique_ptr<Expression> expr)
		: Statement(Statement::ExprStatement),
		  expr(std::move(expr))
	{ }

	virtual ~ExprStatement() = default;

	Expression& expression()
	{
		return *expr;
	}

private:
	std::unique_ptr<Expression> expr;
};

using StatementList = std::vector<std::unique_ptr<Statement>>;

class StatementBlock : public Statement {
public:
	StatementBlock(StatementList statements)
		: Statement(Statement::StatementBlock),
		  stmts(std::move(statements))
	{ }

	virtual ~StatementBlock() = default;

	StatementList& statements()
	{
		return stmts;
	}

private:
	StatementList   stmts;
};

class Declaration;

class DeclStatement : public Statement {
public:
	DeclStatement(std::unique_ptr<Declaration> local)
		: Statement(Statement::DeclStatement),
		  decl(std::move(local))
	{ }

	virtual ~DeclStatement() = default;

	Declaration& declaration()
	{
		return *decl;
	}

protected:
	std::unique_ptr<Declaration> decl;
};

/***************************
 * Control flow statements *
 ***************************/
/*!
 * ifStmt = 'if' '(' expr ')' block optElse
 * optElse = ('else' block) | ϵ
 */
class IfStatement : public Statement {
public:
	IfStatement(std::unique_ptr<Expression> ifExpr,
	            std::unique_ptr<Statement>  ifBody,
	            std::unique_ptr<Statement> elseBody)
		: Statement(Statement::IfStatement),
		  ifExpr(std::move(ifExpr)),
		  ifBody(std::move(ifBody)),
		  elseBody(std::move(elseBody))
	{ }

	virtual ~IfStatement() = default;

	Expression& condition()
	{
		return *ifExpr;
	}

	Statement& thenBranch()
	{
		return *ifBody;
	}

	Statement* elseBranch()
	{
		return elseBody.get();
	}

protected:
	std::unique_ptr<Expression> ifExpr;
	std::unique_ptr<Statement>  ifBody;
	std::unique_ptr<Statement>  elseBody;
};

/*!
 * whileStmt   = 'while' '(' expr ')' stmt
 */
class WhileStatement : public Statement {
public:
	WhileStatement(std::unique_ptr<Expression> cond, std::unique_ptr<Statement> stmt)
		: Statement(Statement::WhileStatement),
		  cond(std::move(cond)),
		  stmt(std::move(stmt))
	{ }

	virtual ~WhileStatement() = default;

	Expression& condition()
	{
		return *cond;
	}

	Statement& body()
	{
		return *stmt;
	}

private:
	std::unique_ptr<Expression> cond;
	std::unique_ptr<Statement>  stmt;
};

/*!
 * DoStmt = 'do' stmt 'while '(' expr ')'
 */
class DoStatement : public Statement {
public:
	DoStatement(std::unique_ptr<Expression> cond, std::unique_ptr<Statement> stmt)
		: Statement(Statement::DoStatement),
		  cond(std::move(cond)),
		  stmt(std::move(stmt))
	{ }

	virtual ~DoStatement() = default;

	Expression& condition()
	{
		return *cond;
	}

	Statement& body()
	{
		return *stmt;
	}

private:
	std::unique_ptr<Expression> cond;
	std::unique_ptr<Statement>  stmt;
};

class BreakStatement : public Statement {
public:
	BreakStatement()
		: Statement(Statement::BreakStatement)
	{ }

	virtual ~BreakStatement() = default;
};

class ContinueStatement : public Statement {
public:
	ContinueStatement()
		: Statement(Statement::ContinueStatement)
	{ }

	virtual ~ContinueStatement() = default;
};

/*!
 * return = 'return' expr 
 */
class ReturnStatement : public Statement {
public:
	ReturnStatement(std::unique_ptr<Expression> retExpr)
		: Statement(Statement::ReturnStatement),
		  expr(std::move(retExpr))
	{ }

	virtual ~ReturnStatement() = default;

	Expression* expression()
	{
		return expr.get();
	}

protected:
	std::unique_ptr<Expression> expr;
};
} // namespace tree
} // namespace daedalus
#endif//Daedalus_Statement
