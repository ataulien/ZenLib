/*
 * Copyright (C) 2016   Hedede <haddayn@gmail.com>
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
#include "StringUtils.h"
namespace Daedalus {
static constexpr char CompositionChar = '%';

std::string compose(
        std::string const& fmt,
        std::vector<std::string> const& args)
{
	constexpr char delim = CompositionChar;

	std::string result;
	result.reserve(fmt.size());

	size_t pos = 0;

	do {
		size_t nextpos = fmt.find(delim, pos);

		result += fmt.substr(pos, nextpos - pos);
		if (nextpos == std::string::npos)
			break;

		char idx = fmt[++nextpos];
		// "%N" is replaced by one of the args
		if (isdigit(idx)) {
			pos = nextpos;

			while (isdigit(fmt[nextpos]))
				++nextpos;

			size_t arg_no = stoull(fmt.substr(pos, nextpos - pos));

			if (arg_no < args.size())
				result += args[arg_no];
		// "%%" is replaced by %, to escape strings like "%N"
		} else if (idx == delim) {
			result += delim;
			++nextpos;
		// Just a single '%' is unmodified
		} else {
			result += delim;
		}
		pos = nextpos;
	} while (pos != std::string::npos);

	return result;
}
} // namespace Daedalus
