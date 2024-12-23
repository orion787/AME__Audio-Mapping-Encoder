#include <string>
#include <vector>
#include <functional>
#include <sstream>
#include <iomanip>

class BlockHasher {
public:
    std::string hashBlock(const std::vector<char>& block) {
        std::hash<std::string> hasher;
        std::size_t hashValue = hasher(std::string(block.begin(), block.end()));

        std::ostringstream oss;
        oss << std::hex << std::setw(sizeof(std::size_t) * 2) << std::setfill('0') << hashValue;

        return oss.str();
    }
};
