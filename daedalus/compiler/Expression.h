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
#ifndef Daedalus_Expression
#define Daedalus_Expression
#include <daedalus/compiler/Statement.h>
namespace Daedalus {
namespace tree {
class Expression {
public:
	virtual ~Expression() = default;

	enum Kind {
		IdentifierExpr,
		FieldExpr,
		CallExpr,
		UnaryExpr,
		BinaryExpr,
		NumberExpr,
		StringExpr,
		SubscriptExpr,
		ArrayInitializer,
	};

	Kind kind() const
	{
		return kind_;
	}

protected:
	Expression(Kind k)
		: kind_(k)
	{ }

private:
	Kind const kind_;
};

/*!
 * Reference to a identifier
 */
class IdentifierExpr : public Expression {
public:
	IdentifierExpr(std::string id)
		: Expression(Expression::IdentifierExpr),
		  id(id)
	{
	}

	virtual ~IdentifierExpr() = default;

	std::string getName()
	{
		return id;
	}

private:
	std::string id;
};

/*!
 * Reference to a class field
 */
class FieldExpr : public Expression {
public:
	FieldExpr(std::string inst,
	          std::string field)
		: Expression(Expression::FieldExpr),
		  inst(inst), field(field)
	{
	}

	virtual ~FieldExpr() = default;

	std::string identifier() const
	{
		return inst;
	}

	std::string fieldName()
	{
		return field;
	}

private:
	std::string inst;
	std::string field;
};

class CallExpr : public Expression {
public:
	CallExpr(std::string callee,
	         std::vector<std::unique_ptr<Expression>> arguments)
		: Expression(Expression::CallExpr),
		  callee(callee), argList(std::move(arguments))
	{
	}

	virtual ~CallExpr() = default;

	std::string function() const
	{
		return callee;
	}

	std::vector<std::unique_ptr<Expression>>& arguments()
	{
		return argList;
	}

private:
	//Function* callee;
	std::string callee;
	std::vector<std::unique_ptr<Expression>> argList;
};

/*!
 * unary = op primary
 */
class UnaryExpr : public Expression {
public:
	UnaryExpr(int op, std::unique_ptr<Expression> operand)
		: Expression(Expression::UnaryExpr),
		  op(op), operand(std::move(operand))
	{
	}

	virtual ~UnaryExpr() = default;

	Expression& getOperand()
	{
		return *operand.get();
	}

	int getOperation()
	{
		return op;
	}

private:
	int op;
	std::unique_ptr<Expression> operand;
};

/*!
 * binary = unary op unary
 */
class BinaryExpr : public Expression {
public:
	BinaryExpr(int op,
	           std::unique_ptr<Expression> lhs,
	           std::unique_ptr<Expression> rhs)
		: Expression(Expression::BinaryExpr), op(op),
		  lhs(std::move(lhs)), rhs(std::move(rhs))
	{ }

	virtual ~BinaryExpr() = default;

	Expression& getLHS()
	{
		return *lhs.get();
	}
	Expression& getRHS()
	{
		return *rhs.get();
	}
	int getOperation()
	{
		return op;
	}

private:
	int op;
	std::unique_ptr<Expression> lhs;
	std::unique_ptr<Expression> rhs;
};

/*!
 * Numeric literal (int or float)
 */
class NumberExpr : public Expression {
public:
	NumberExpr(std::string value)
		: Expression(Expression::NumberExpr),
		  value(value)
	{
	}

	virtual ~NumberExpr() = default;

	std::string getValue() const
	{
		return value;
	}

private:
	std::string value;
};

/*!
 * String literal (double-quoted "")
 */
class StringExpr : public Expression {
public:
	StringExpr(std::string value)
		: Expression(Expression::StringExpr),
		  value(value)
	{ }

	virtual ~StringExpr() = default;

	std::string getValue() const
	{
		return value;
	}

private:
	std::string value;
};

class SubscriptExpr : public Expression {
public:
	SubscriptExpr(std::unique_ptr<Expression> array,
	              std::unique_ptr<Expression> expr)
		: Expression(Expression::SubscriptExpr),
		  arrayExpr(std::move(array)),
		  subscriptExpr(std::move(expr))
	{
	}

	virtual ~SubscriptExpr() = default;

	Expression& array() const
	{
		return *arrayExpr;
	}

	Expression& subscript()
	{
		return *subscriptExpr;
	}

private:
	std::unique_ptr<Expression> arrayExpr;
	std::unique_ptr<Expression> subscriptExpr;
};

// TODO: Probably initializer should be a class on its own
class ArrayInitializer : public Expression {
public:
	typedef std::vector<std::unique_ptr<Expression>> InitList;

	ArrayInitializer(InitList initList)
		: Expression(Expression::ArrayInitializer),
		  exprs(std::move(initList))
	{
	}

	virtual ~ArrayInitializer() = default;

	InitList& initList()
	{
		return exprs;
	}

	size_t size()
	{
		return exprs.size();
	}

private:
	InitList exprs;
};
} // namespace tree
} // namespace daedalus
#endif//Daedalus_Expression
