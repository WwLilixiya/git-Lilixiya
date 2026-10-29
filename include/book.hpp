#pragma once

#include <iosfwd>
#include <string>

namespace library {

// 一本书的馆藏记录。所有副本共用一个编号，不追踪具体借阅人。
class Book {
public:
    Book(int id, std::string title, std::string author, int year,
         int total_copies, int available_copies);

    int id() const noexcept { return id_; }
    const std::string& title() const noexcept { return title_; }
    const std::string& author() const noexcept { return author_; }
    int year() const noexcept { return year_; }
    int total_copies() const noexcept { return total_copies_; }
    int available_copies() const noexcept { return available_copies_; }

    bool borrow() noexcept;
    bool return_copy() noexcept;

private:
    int id_;
    std::string title_;
    std::string author_;
    int year_;
    int total_copies_;
    int available_copies_;
};

std::ostream& operator<<(std::ostream& out, const Book& book);

}  // namespace library
