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
#ifndef Daedalus_Printer
#define Daedalus_Printer
#include <aw/io/WriteStream.h>
namespace daedalus {
using namespace aw;
class Printer {
public:
	Printer(io::WriteStream& out);
	~Printer() = default;

	void visit(tree::Declaration& node);

	void visit(tree::FunctionProto& node);
	void visit(tree::Function& node);
	void visit(tree::Variable& node);
	void visit(tree::Class& node);
	void visit(tree::Prototype& node);
	void visit(tree::Instance& node);

	void visit(tree::Statement& node);

	void visit(tree::DeclStatement& node);
	void visit(tree::StatementBlock& node);
	void visit(tree::IfStatement& node);
	void visit(tree::ReturnStatement& node);
	void visit(tree::WhileStatement& node);
	void visit(tree::DoStatement& node);
	void visit(tree::BreakStatement& node);
	void visit(tree::ContinueStatement& node);

	void visit(tree::Expression& node);

	void visit(tree::NumberExpr& node);
	void visit(tree::StringExpr& node);
	void visit(tree::IdentifierExpr& node);
	void visit(tree::ArrayInitializer& node);
	void visit(tree::CallExpr& node);
	void visit(tree::FieldExpr& node);
	void visit(tree::SubscriptExpr& node);
	void visit(tree::UnaryExpr& node);
	void visit(tree::BinaryExpr& node);

private:
	void printSignature(tree::FunctionProto& node);
	void write(char c);
	void write(std::string s);
	void writeInline(char c);
	void writeInline(std::string s);
	void startInline(std::string name);
	void endInline();
	void start(std::string name = "");
	void end();
	void startLine();
	void endLine();

	io::WriteStream& writer;
	size_t depth = 0;
	enum State {
		LineStart,
		Middle,
		LineEnd,
	} state;
};
} // namespace daedalus
#endif//Daedalus_Printer
