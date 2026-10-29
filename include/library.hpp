#pragma once

#include "storage.hpp"

#include <memory>
#include <string>
#include <string_view>
#include <vector>

namespace library {

enum class SortField { Id, Title, Year };

struct Statistics {
    std::size_t titles = 0;
    int total_copies = 0;
    int available_copies = 0;
    int borrowed_copies() const noexcept { return total_copies - available_copies; }
};

class Library {
public:
    explicit Library(std::unique_ptr<IStorage> storage);

    void load();
    void save() const;
    int add_book(std::string title, std::string author, int year, int copies);
    bool remove_book(int id);
    bool borrow_book(int id);
    bool return_book(int id);

    Book* find_by_id(int id) noexcept;
    const Book* find_by_id(int id) const noexcept;
    std::vector<Book> search(std::string_view keyword) const;
    std::vector<Book> sorted_books(SortField field) const;
    Statistics statistics() const noexcept;

private:
    CatalogSnapshot catalog_;
    std::unique_ptr<IStorage> storage_;
};

}  // namespace library
