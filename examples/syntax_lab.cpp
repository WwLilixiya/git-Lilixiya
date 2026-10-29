#include <array>
#include <cassert>
#include <iostream>
#include <map>
#include <numeric>
#include <optional>
#include <string>

// 同一图书项目的补充实验：书架编号、每架藏书量与可借比例。
namespace {

using ShelfId = int;  // 类型别名不会创建新类型。
constexpr std::size_t kShelfCount = 3;
static_assert(kShelfCount > 0, "A library needs at least one shelf");

std::optional<std::string> find_shelf(const std::map<ShelfId, std::string>& shelves,
                                      ShelfId id) {
    const auto it = shelves.find(id);
    if (it == shelves.end()) return std::nullopt;
    return it->second;  // 转换为拥有字符串的 optional，避免悬空引用。
}

}  // namespace

int main() {
    std::array<int, kShelfCount> copies{{3, 2, 5}};
    std::map<ShelfId, std::string> shelves{{1, "Programming"},
                                            {2, "Algorithms"},
                                            {3, "History"}};

    const int total = std::accumulate(copies.begin(), copies.end(), 0);
    const int available = copies.at(0) + copies.at(1);
    const double ratio = static_cast<double>(available) / total;
    assert(total == 10 && ratio == 0.5);

    for (const auto& [id, name] : shelves) {
        std::cout << "Shelf " << id << ": " << name << '\n';
    }
    for (std::size_t i = 0; i < copies.size(); ++i) {
        std::cout << "Shelf " << i + 1 << " copies: " << copies.at(i) << '\n';
    }
    if (const auto found = find_shelf(shelves, 2)) {
        std::cout << "Found: " << *found << '\n';
    }
    std::cout << "Available ratio: " << ratio << '\n';
}
