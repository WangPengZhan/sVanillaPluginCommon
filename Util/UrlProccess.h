#include <string>

namespace util
{
bool isUri(const std::string& uri);
std::string u8ToString(const std::u8string& u8Str);
std::string urlDecode(const std::string& encoded);
}