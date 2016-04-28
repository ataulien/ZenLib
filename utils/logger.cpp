#include "logger.h"

#ifdef USE_LOG

// Definition of the static log callback
std::function<void(const std::string&)> Utils::Log::s_LogCallback;
std::string Utils::Log::s_LogFile;

#endif
