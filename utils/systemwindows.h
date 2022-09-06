#pragma once
#include <windows.h>

namespace ZenLib
{
    namespace Utils
    {
        class System
        {
        public:
            /**
		 * @brief Creates a directory on the systems file structure
		 */
            static void mkdir(const char* path)
            {
                CreateDirectory(path, nullptr);
            }
        };
    }  // namespace Utils
}  // namespace ZenLib
