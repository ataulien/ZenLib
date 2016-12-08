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
#ifndef Daedalus_SymbolTable
#define Daedalus_SymbolTable
#include <limits>
#include <unordered_map>
#include <daedalus/compiler/Symbol.h>
namespace Daedalus {
struct Scope {
	std::vector<Variable> variables;

	// daedalus doesn't support non-top-level class/func,
	// but interpreter doesn't care about that
	std::vector<Function> functions;
	std::vector<Class>    classes;

	std::vector<Instance>  instances;
	std::vector<Prototype> prototypes;
};


struct SymbolTable {
	SymbolTable();

	SymbolRef getSymbol(std::string name);
	Scope*    getScope(SymbolRef ref);
	Symbol*   getSymbol(SymbolRef ref);

	unsigned pushScope();
	unsigned currentScope();
	void popScope();

	enum InsertResult {
		Success,
		AlreadyDefined,
		NotAllowedHere,
	};

	SymbolRef insertVariable(Variable&& var);
	SymbolRef insertClass(Class&& type);
	SymbolRef insertFunction(Function&& func);
	SymbolRef insertInstance(Instance&& func);
	SymbolRef insertPrototype(Prototype&& func);

private:
	using SymbolMap = std::unordered_map<std::string, SymbolRef>;

	SymbolMap table;

	void saveSymbol(std::string name);
	void insertSymbol(std::string name, SymbolRef ref);

	using Entry = std::pair<std::string, SymbolRef>;
	using EntryList = std::vector<Entry>;
	std::vector<EntryList> tableDeltas;

	std::vector<Scope> scopes;
	std::vector<unsigned> scopeStack;
};
} // namespace daedalus
#endif//Daedalus_SymbolTable
