#pragma once

#include <sstream>
#include <string>
#include <vector>

namespace ZenLib
{
    namespace Utils
    {
        std::vector<std::string>& split(const std::string& s, const char delim, std::vector<std::string>& elems);

        /**
     * @brief split string function
     * @param string
     * @param delimitor
     * @return splitted string in a std::vector<std::string>
     */
        std::vector<std::string> split(const std::string& s, const char delim);

        /**
	 * @brief Splits a string on every occasion of one of the input-chars, in the given order
	 *		  Undefined behavior if the chars occur multiple times.
	 */
        std::vector<std::string> split(const std::string& s, const std::string& delim);

        std::string replaceString(std::string subject, const std::string& search, const std::string& replace);
    }  // namespace Utils
}  // namespace ZenLib
