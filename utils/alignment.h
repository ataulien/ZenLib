#pragma once
#include <inttypes.h>
#include <string.h>

#if EMSCRIPTEN
#include <emscripten.h>
#endif

namespace Utils
{
#if EMSCRIPTEN
    typedef emscripten_align1_int unaligned_int32;
    typedef emscripten_align1_short unaligned_int16;
    typedef emscripten_align1_float unaligned_float;
#else
    typedef int32_t unaligned_int32;
    typedef int16_t unaligned_int16;
    typedef float unaligned_float;
#endif
	/**
	 * @brief performs an unaligned memory-access
	 */
	template<typename T>
	static void getUnaligned(T* dst, const void* src)
	{
		memcpy(dst, src, sizeof(T));
	}

    template<typename T>
    void unalignedRead(T& dst, const void*& src)
    {
        getUnaligned(&dst, src);
        uint8_t*& ptr = ((uint8_t*&)src);
        ptr += sizeof(T);
    }
}
