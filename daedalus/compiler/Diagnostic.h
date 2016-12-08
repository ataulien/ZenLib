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
#ifndef Daedalus_Diagnostic
#define Daedalus_Diagnostic
#include <daedalus/compiler/PrintToken.h>
namespace Daedalus {
class DiagnosticsEngine;

/*!
 * This class is used to compose diagnostic messages
 */
class Diagnostic {
public:
	enum ID {
#define DIAG(type, id, msg) id,
#include <daedalus/compiler/Messages.h>
#undef DIAG
	};

	Diagnostic(Location loc, ID id)
		: loc(loc), id(id)
	{
	}

	Diagnostic& arg(std::string str)
	{
		args.push_back(str);
		return *this;
	}
private:
	friend class DiagnosticHelper;

	Location loc;
	ID id;
	std::vector<std::string> args;
};

char const* diagMessages[] = {
#define DIAG(type, id, msg) msg,
#include <daedalus/compiler/Messages.h>
#undef DIAG
};

Diagnostic& operator << (Diagnostic& diag, std::string str)
{
	return diag.arg(str);
}

Diagnostic& operator << (Diagnostic& diag, Token::Kind type)
{
	return diag.arg(spellToken(type));
}
} // namespace daedalus
#endif//Daedalus_Diagnostic
