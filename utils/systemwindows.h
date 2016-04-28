#pragma once
#include <Windows.h>

namespace Utils
{
    class System
    {
    public:
		/**
		 * @brief Creates a directory on the systems file structure
		 */
        static void mkdir(const char *path)
        {
			CreateDirectory(path, nullptr);
        }
    };
}

