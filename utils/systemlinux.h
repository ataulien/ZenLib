#pragma once

#include <sys/stat.h>

namespace ZenLib
{
    namespace Utils
    {
        class System
        {
        public:
            static void mkdir(const char* path)
            {
                ::mkdir(path, S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
            }
        };
    }  // namespace Utils
}  // namespace ZenLib
