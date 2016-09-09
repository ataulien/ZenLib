#pragma once

#if defined(WIN32) || defined(_WIN32)
#include "systemwindows.h"
#else
#include "systemlinux.h"
#define SYS Utils::System
#endif
