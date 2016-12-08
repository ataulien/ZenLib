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
#ifndef Daedalus_Location
#define Daedalus_Location
#include <daedalus/compiler/SourceBuffer.h>
namespace Daedalus {
std::pair<size_t,size_t> countLines(SourceBuffer& buf, std::ptrdiff_t offset)
{
	auto pos  = std::begin(buf);
	auto end  = pos + offset;
	auto last = pos;

	size_t row = 1;

	while (pos++ < end) {
		if (*pos == '\n') {
			last = pos;
			++row;
		}
	}

	auto column = end - last + 1;

	return {row, column};
}
} // namespace daedalus
#endif//Daedalus_Location
