#include "book.hpp"

#include <algorithm>
#include <cctype>
#include <ostream>
#include <stdexcept>
#include <utility>

namespace library {
namespace {

bool valid_text(const std::string& value) {
    // std::isspace 接收 unsigned char 范围的值；直接传有符号 char 可能出错。
    const auto non_space = [](unsigned char ch) { return !std::isspace(ch); };
    return std::any_of(value.begin(), value.end(), non_space)
        && value.find_first_of("\r\n") == std::string::npos;
}

}  // namespace

Book::Book(int id, std::string title, std::string author, int year,
           int total_copies, int available_copies)
    : id_(id), title_(std::move(title)), author_(std::move(author)),
      year_(year), total_copies_(total_copies),
      available_copies_(available_copies) {
    if (id_ <= 0 || !valid_text(title_) || !valid_text(author_) ||
        year_ < 0 || year_ > 2100 || total_copies_ <= 0 ||
        available_copies_ < 0 || available_copies_ > total_copies_) {
        throw std::invalid_argument("Invalid book data");
    }
}

bool Book::borrow() noexcept {
    if (available_copies_ == 0) return false;
    --available_copies_;
    return true;
}

bool Book::return_copy() noexcept {
    if (available_copies_ == total_copies_) return false;
    ++available_copies_;
    return true;
}

std::ostream& operator<<(std::ostream& out, const Book& book) {
    return out << '#' << book.id() << " | " << book.title() << " | "
               << book.author() << " | " << book.year() << " | "
               << book.available_copies() << '/' << book.total_copies()
               << " available";
}

}  // namespace library
