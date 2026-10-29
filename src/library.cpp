#include "library.hpp"

#include <algorithm>
#include <limits>
#include <stdexcept>
#include <utility>

namespace library {

Library::Library(std::unique_ptr<IStorage> storage) : storage_(std::move(storage)) {
    if (!storage_) throw std::invalid_argument("Storage is required");
}

void Library::load() { catalog_ = storage_->load(); }
void Library::save() const { storage_->save(catalog_); }

int Library::add_book(std::string title, std::string author, int year, int copies) {
    if (catalog_.next_id == std::numeric_limits<int>::max()) {
        throw std::overflow_error("Book ID limit reached");
    }
    const int id = catalog_.next_id;
    // emplace_back 成功后才更新编号；构造失败时状态保持不变。
    catalog_.books.emplace_back(id, std::move(title), std::move(author),
                                year, copies, copies);
    ++catalog_.next_id;
    return id;
}

Book* Library::find_by_id(int id) noexcept {
    const auto it = std::find_if(catalog_.books.begin(), catalog_.books.end(),
                                 [id](const Book& book) { return book.id() == id; });
    return it == catalog_.books.end() ? nullptr : &*it;
}

const Book* Library::find_by_id(int id) const noexcept {
    const auto it = std::find_if(catalog_.books.cbegin(), catalog_.books.cend(),
                                 [id](const Book& book) { return book.id() == id; });
    return it == catalog_.books.cend() ? nullptr : &*it;
}

bool Library::remove_book(int id) {
    const Book* book = find_by_id(id);
    if (!book || book->available_copies() != book->total_copies()) return false;
    const auto end = std::remove_if(catalog_.books.begin(), catalog_.books.end(),
                                    [id](const Book& item) { return item.id() == id; });
    catalog_.books.erase(end, catalog_.books.end());
    return true;
}

bool Library::borrow_book(int id) {
    Book* book = find_by_id(id);
    return book && book->borrow();
}

bool Library::return_book(int id) {
    Book* book = find_by_id(id);
    return book && book->return_copy();
}

std::vector<Book> Library::search(std::string_view keyword) const {
    std::vector<Book> result;
    for (const Book& book : catalog_.books) {
        if (book.title().find(keyword) != std::string::npos ||
            book.author().find(keyword) != std::string::npos) {
            result.push_back(book);
        }
    }
    return result;
}

std::vector<Book> Library::sorted_books(SortField field) const {
    auto result = catalog_.books;  // 副本：排序不会改变存储顺序。
    std::sort(result.begin(), result.end(), [field](const Book& a, const Book& b) {
        switch (field) {
            case SortField::Title: return a.title() < b.title();
            case SortField::Year: return a.year() < b.year();
            case SortField::Id: return a.id() < b.id();
        }
        return a.id() < b.id();
    });
    return result;
}

Statistics Library::statistics() const noexcept {
    Statistics stats{catalog_.books.size(), 0, 0};
    for (const auto& book : catalog_.books) {
        stats.total_copies += book.total_copies();
        stats.available_copies += book.available_copies();
    }
    return stats;
}

}  // namespace library
