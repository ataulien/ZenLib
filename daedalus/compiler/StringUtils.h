#ifndef Daedalus_StringUtils
#define Daedalus_StringUtils
#include <algorithm>
#include <cstring>
#include <string>
#include <vector>
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

std::string compose(std::string const& fmt, std::vector<std::string> const& args);

/*!
 * Simpler replacement for printf:
 * compose("%0 is a %1.", "This", "string"); -> "This is a string"
 */
template<typename... Args>
std::string compose(std::string const& fmt, Args const&... args)
{
	using std::to_string;
	std::vector<std::string> bits{to_string(args)...};

	return compose(fmt, bits);
}
} // namespace Daedalus
#endif//Daedalus_StringUtils
