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
#ifndef Daedalus_Symbol
#define Daedalus_Symbol
#include <vector>
#include <limits>
#include <daedalus/compiler/Type.h>
namespace Daedalus {
struct Symbol {
	static constexpr unsigned undefined = std::numeric_limits<unsigned>::max();

	enum Kind {
		Undefined,
		Variable,
		Function,
		Class,
		Instance,
		Prototype,
		NumKinds
	};

	Symbol(std::string name, Kind kind)
		: name(name), kind(kind)
	{}

	std::string name;
	Kind kind;
};

struct SymbolRef {
	SymbolRef()
		: kind(Symbol::Undefined)
	{}

	SymbolRef(Symbol::Kind kind)
		: kind(kind), scopeId{0}, index{0}
	{}

	SymbolRef(Symbol::Kind kind, size_t scope, size_t index)
		: kind(kind), scopeId(scope), index(index)
	{}

	bool isValid()
	{
		return kind != Symbol::Undefined;
	}

	std::uint64_t kind    : 3;
	std::uint64_t scopeId : 24;
	std::uint64_t index   : 29;
};

using SymbolRefList = std::vector<SymbolRef>;

struct Variable : Symbol {
	Variable(std::string name, Type type)
		: Symbol(name, Symbol::Variable), type(type)
	{}
	Type type;
};

struct Function : Symbol {
	Function(std::string name, Type ret)
		: Symbol(name, Symbol::Function), returnType(ret)
	{}

	Type returnType;
	std::vector<unsigned> args;
	unsigned scope = Symbol::undefined;
};

struct Prototype : Symbol {
	SymbolRef parent;
	unsigned scope = Symbol::undefined;
};

struct Instance : Symbol {
	SymbolRef parent;
	unsigned scope = Symbol::undefined;
};

enum class TypeID {
	Void,
	Integer,
	Float,
	String,
	FuncRef,
	Function,
	Class
};

struct Class : Symbol {
	Class(std::string name, TypeID id)
		: Symbol(name, Symbol::Class), id(id)
	{ }

	Class(std::string name, unsigned scope)
		: Symbol(name, Symbol::Class), id(TypeID::Class), scope(scope)
	{ }

	TypeID id;
	unsigned scope = Symbol::undefined;
};
} // namespace daedalus
#endif//Daedalus_Symbol
