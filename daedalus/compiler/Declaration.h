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
#ifndef Daedalus_Declaration
#define Daedalus_Declaration
#include <vector>
#include <memory>
#include <daedalus/compiler/Symbol.h>
namespace Daedalus {
using StringList = std::vector<std::string>;
namespace tree {
/*struct Type {
	std::string name;
	bool isConst;
	unsigned count;
};*/

/*!
 * Declaration specifies meaning of a symbol.
 */
class Declaration {
public:
	virtual ~Declaration() = default;

	//! List of concrete derived types
	enum Kind {
		FunctionProto,
		Function,
		Variable,
		Class,
		Prototype,
		Instance
	};

	Kind kind() const
	{
		return kind_;
	}

protected:
	Declaration(Kind k)
		: kind_(k)
	{ }

private:
	Kind const kind_;
};

using DeclarationList = std::vector<std::unique_ptr<Declaration>>;

class Expression;

struct Variable : Declaration {
	Variable(std::string id, Type type)
		: Declaration(Declaration::Variable),
		  id(id), _type(type)
	{ }

	virtual ~Variable() = default;

	std::string name() const
	{
		return id;
	}

	Type type() const
	{
		return _type;
	}

	bool isArray() const
	{
		return _type.count > 1;
	}

	bool isConst()
	{
		return _type.isConst;
	}

	void setConst(bool cnst)
	{
		_type.isConst = cnst;
	}

	Expression* initializer()
	{
		return init_expr.get();
	}

	void setInitialier(std::unique_ptr<Expression> newVal)
	{
		init_expr = std::move(newVal);
	}

	// TODO: ArrayDecl : public Variable?
	// XXX: nope, will be part of type system
	Expression* sizeExpr() const
	{
		return size_expr.get();
	}

	void setSizeExpr(std::unique_ptr<Expression> newSize)
	{
		size_expr = std::move(newSize);
	}

private:
	std::string id;
	Type _type;
	std::unique_ptr<Expression> init_expr = nullptr;
	std::unique_ptr<Expression> size_expr = nullptr;
};

typedef std::vector<std::unique_ptr<Variable>> VarList;

// TODO: merge with Function
struct FunctionProto : Declaration {
	FunctionProto(std::string id, Type returnType, VarList args)
		: Declaration(Declaration::FunctionProto),
		  _name(id), _returnType(returnType), args(std::move(args))
	{ }

	virtual ~FunctionProto() = default;

	std::string name() const
	{
		return _name;
	}

	Type returnType() const
	{
		return _returnType;
	}

	VarList& arguments()
	{
		return args;
	}

private:
	std::string _name;
	Type _returnType;
	VarList args;
};

class StatementBlock;

struct Function : Declaration {
	Function(std::unique_ptr<tree::FunctionProto> proto,
		 std::unique_ptr<StatementBlock> body)
		: Declaration(Declaration::Function),
		  proto(std::move(proto)),
		  body(std::move(body))
	{ }

	virtual ~Function() = default;

	tree::FunctionProto& prototype() const
	{
		return *proto;
	}

	StatementBlock& getBody()
	{
		return *body;
	}

	/*
	void addLocal(std::unique_ptr<Variable> var)
	{
		localVars.push_back(std::move(var));
	}

	VarList& locals()
	{
		return localVars;
	}
	*/

private:
	std::unique_ptr<tree::FunctionProto> proto;
	std::unique_ptr<StatementBlock> body;
	//VarList localVars;
};

class Class : public Declaration {
public:
	Class(std::string name, VarList members)
		: Declaration(Declaration::Class),
		  name(name), body(std::move(members))
	{ }

	virtual ~Class() = default;

	std::vector<std::unique_ptr<tree::Variable>>& members()
	{
		return body;
	}

private:
	std::string name;
	std::vector<std::unique_ptr<tree::Variable>> body;
};

class Prototype : public Declaration {
public:
	Prototype(std::string name, std::string base,
	          std::unique_ptr<StatementBlock> body)
		: Declaration(Declaration::Prototype),
		  name_(name), base_(base), block(std::move(body))
	{
	}

	virtual ~Prototype() = default;

	std::string name() const
	{
		return name_;
	}

	std::string base() const
	{
		return base_;
	}

	StatementBlock& body() const
	{
		return *block;
	}

protected:
	std::string name_;
	std::string base_;

	std::unique_ptr<StatementBlock> block;
};

struct Instance : Declaration {
	Instance(StringList&& names, std::string base,
	          std::unique_ptr<StatementBlock> body)
		: Declaration(Declaration::Instance),
		  names(names), base_(base), block(std::move(body))
	{ }

	virtual ~Instance() = default;

	std::string name() const
	{
		return names.front();
	}

	StringList const& nameList() const
	{
		return names;
	}

	std::string base() const
	{
		return base_;
	}

	StatementBlock* body() const
	{
		return block.get();
	}

protected:
	StringList names;
	std::string base_;

	std::unique_ptr<StatementBlock> block;
};
} // namespace tree
} // namespace daedalus
#endif//Daedalus_Declaration
