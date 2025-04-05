#include <string>

namespace util
{
static constexpr char32_t specialChars[] = {U'?', U'/', U'"', U'\\', U'<', U'>', U'*', U'|', U':', 0x2022};
bool isUri(const std::string& uri);
std::string u8ToString(const std::u8string& u8Str);
std::string urlDecode(const std::string& encoded);
std::string removeSpecialChars(const std::string& str);
}  // namespace util
