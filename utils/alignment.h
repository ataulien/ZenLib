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
		uint8_t* dst_ptr = reinterpret_cast<uint8_t*>(dst);
		memcpy(dst_ptr, src, sizeof(T));
	}
}