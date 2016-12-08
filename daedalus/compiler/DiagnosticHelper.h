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
#ifndef Daedalus_DignosticHelper
#define Daedalus_DignosticHelper
#include <cassert>
#include <iostream> //temporary
#include <daedalus/compiler/Location.h>
#include <daedalus/compiler/Diagnostic.h>
namespace Daedalus {
struct DiagnosticHelper {
	DiagnosticHelper()
	{
	}

	DiagnosticHelper(SourceBuffer& inputBuffer)
		: buf(&inputBuffer)
	{
	}

	// temporary
	SourceBuffer* buf;

	void report(Diagnostic const& diag)
	{
		assert(size_t(diag.id) < sizeof(diagMessages)/sizeof(char*));
		// TODO: pull compose out of awlib
		//auto msg = string::compose(diagMessages[diag.id], diag.args);
		auto msg = std::string();

		auto pos = countLines(*buf, diag.loc.pos);

		auto line   = pos.first;
		auto column = pos.second;

		std::cerr << "error:" << line << ":" << column << ": " << msg << "\n";
	}
};
} // namespace daedalus
#endif//Daedalus_DignosticHelper
