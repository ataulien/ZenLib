#ifndef Daedalus_StringUtils
#define Daedalus_StringUtils
#include <algorithm>
#include <cstring>
#include <string>
namespace Daedalus {
/*!
 * Transform the string to lower case.
 */
inline std::string& tolower(std::string& str)
{
	auto lower_char = [] (char c) { return std::tolower(c); };
	std::transform(begin(str), end(str), begin(str), lower_char);
	return str;
}
} // namespace Daedalus
#endif//Daedalus_StringUtils
