#include <string>
#include <unordered_set>

namespace Bit {
namespace Jobs {

std::unordered_set<std::string> ParseDeps(const std::string &deps_str);

} // Jobs
} // Jhm
