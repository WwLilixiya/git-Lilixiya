#include "library.hpp"

#include <iostream>
#include <limits>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace {

std::string read_line(const std::string& prompt) {
    std::cout << prompt;
    std::string value;
    if (!std::getline(std::cin, value)) throw std::runtime_error("Input closed");
    return value;
}

// 函数模板：菜单选项、编号、年份和册数共用输入校验。
template <typename T>
T read_number(const std::string& prompt) {
    while (true) {
        std::istringstream input(read_line(prompt));
        T value{};
        if (input >> value) {
            input >> std::ws;
            if (input.eof()) return value;
        }
        std::cout << "Please enter one valid number.\n";
    }
}

void print_books(const std::vector<library::Book>& books) {
    if (books.empty()) {
        std::cout << "No books found.\n";
        return;
    }
    for (const auto& book : books) std::cout << book << '\n';
}

void print_menu() {
    std::cout << "\n=== Library Manager ===\n"
              << "1 Add book\n2 List books\n3 Search\n4 Borrow\n"
              << "5 Return\n6 Remove book\n7 Statistics\n0 Exit\n";
}

}  // namespace

int main(int argc, char* argv[]) {
    try {
        // argv[1] 可指定数据文件；默认 books.db 位于当前工作目录。
        const std::string path = argc > 1 ? argv[1] : "books.db";
        library::Library catalog(std::make_unique<library::TextFileStorage>(path));
        catalog.load();

        while (true) {
            print_menu();
            const int choice = read_number<int>("Choice: ");
            bool changed = false;
            switch (choice) {
                case 0:
                    std::cout << "Goodbye.\n";
                    return 0;
                case 1: {
                    auto title = read_line("Title: ");
                    auto author = read_line("Author: ");
                    const int year = read_number<int>("Year (0-2100): ");
                    const int copies = read_number<int>("Copies (>0): ");
                    const int id = catalog.add_book(std::move(title), std::move(author),
                                                     year, copies);
                    std::cout << "Added book #" << id << ".\n";
                    changed = true;
                    break;
                }
                case 2: {
                    const int sort = read_number<int>("Sort: 1 ID, 2 title, 3 year: ");
                    const auto field = sort == 2 ? library::SortField::Title
                                     : sort == 3 ? library::SortField::Year
                                                 : library::SortField::Id;
                    print_books(catalog.sorted_books(field));
                    break;
                }
                case 3:
                    print_books(catalog.search(read_line("Keyword: ")));
                    break;
                case 4: {
                    const int id = read_number<int>("Book ID: ");
                    changed = catalog.borrow_book(id);
                    std::cout << (changed ? "Borrowed.\n" : "Not found or unavailable.\n");
                    break;
                }
                case 5: {
                    const int id = read_number<int>("Book ID: ");
                    changed = catalog.return_book(id);
                    std::cout << (changed ? "Returned.\n" : "Not found or none borrowed.\n");
                    break;
                }
                case 6: {
                    const int id = read_number<int>("Book ID: ");
                    changed = catalog.remove_book(id);
                    std::cout << (changed ? "Removed.\n" : "Not found or copies borrowed.\n");
                    break;
                }
                case 7: {
                    const auto stats = catalog.statistics();
                    std::cout << "Titles: " << stats.titles
                              << ", copies: " << stats.total_copies
                              << ", available: " << stats.available_copies
                              << ", borrowed: " << stats.borrowed_copies() << '\n';
                    break;
                }
                default:
                    std::cout << "Unknown choice.\n";
            }
            if (changed) catalog.save();  // 每次成功修改后保存。
        }
    } catch (const std::exception& error) {
        std::cerr << "Error: " << error.what() << '\n';
        return 1;
    }
}
