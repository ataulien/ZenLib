#pragma once
#include <inttypes.h>
#include <string.h>

namespace Utils
{

	/**
	 * @brief performs an unaligned memory-access
	 */
	template<typename T>
	static void getUnaligned(T* dst, const void* src)
	{
		memcpy(dst, src, sizeof(T));
	}
}
