#include "library.hpp"

#include <cassert>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>

namespace {

class MemoryStorage final : public library::IStorage {
public:
    mutable library::CatalogSnapshot data;
    library::CatalogSnapshot load() const override { return data; }
    void save(const library::CatalogSnapshot& value) const override {
        data = value;
    }
};

void test_workflow() {
    auto storage = std::make_unique<MemoryStorage>();
    MemoryStorage* storage_view = storage.get();  // 非拥有指针；由 Library 持有对象。
    library::Library catalog(std::move(storage));
    catalog.load();

    const int cpp = catalog.add_book("C++ Primer", "Lippman", 2012, 2);
    const int algo = catalog.add_book("Algorithms", "Sedgewick", 2011, 1);
    assert(cpp == 1 && algo == 2);
    assert(catalog.search("Primer").size() == 1);
    assert(catalog.sorted_books(library::SortField::Title).front().id() == algo);
    assert(catalog.borrow_book(cpp));
    assert(catalog.borrow_book(cpp));
    assert(!catalog.borrow_book(cpp));
    assert(!catalog.remove_book(cpp));
    assert(catalog.statistics().borrowed_copies() == 2);
    assert(catalog.return_book(cpp));
    assert(catalog.return_book(cpp));
    assert(!catalog.return_book(cpp));
    assert(catalog.remove_book(cpp));
    assert(!catalog.remove_book(999));
    catalog.save();
    assert(storage_view->data.books.size() == 1);
    assert(storage_view->data.next_id == 3);
    catalog.load();
    assert(catalog.find_by_id(algo) != nullptr);
    assert(catalog.find_by_id(cpp) == nullptr);
}

void test_validation_and_file(const std::string& path) {
    bool rejected = false;
    try {
        library::Book invalid(1, "  ", "Author", 2020, 1, 1);
    } catch (const std::invalid_argument&) {
        rejected = true;
    }
    assert(rejected);

    {
        library::Library catalog(std::make_unique<library::TextFileStorage>(path));
        catalog.load();
        catalog.add_book("A \"quoted\" title", "Writer", 2024, 3);
        catalog.borrow_book(1);
        catalog.save();
    }
    {
        library::Library catalog(std::make_unique<library::TextFileStorage>(path));
        catalog.load();
        const auto* book = catalog.find_by_id(1);
        assert(book && book->title() == "A \"quoted\" title");
        assert(book->available_copies() == 2);
    }
    {
        std::ofstream corrupt(path, std::ios::trunc);
        corrupt << "LIBRARY_V1 2\n1 \"A\" \"B\" 2020 1 1\n"
                   "1 \"C\" \"D\" 2020 1 1\n";
    }
    rejected = false;
    try {
        library::Library catalog(std::make_unique<library::TextFileStorage>(path));
        catalog.load();
    } catch (const std::runtime_error&) {
        rejected = true;
    }
    assert(rejected);
    std::remove(path.c_str());
}

}  // namespace

int main(int argc, char* argv[]) {
    assert(argc == 2);
    test_workflow();
    test_validation_and_file(argv[1]);
    std::cout << "All tests passed.\n";
}
