#include "storage.hpp"

#include <cerrno>
#include <cstdio>
#include <fstream>
#include <iomanip>
#include <limits>
#include <set>
#include <sstream>
#include <stdexcept>
#include <utility>

namespace library {

TextFileStorage::TextFileStorage(std::string path) : path_(std::move(path)) {
    if (path_.empty()) throw std::invalid_argument("Storage path is empty");
}

CatalogSnapshot TextFileStorage::load() const {
    // 区分“首次启动无文件”和“文件存在但无权限读取”。
    errno = 0;
    std::FILE* probe = std::fopen(path_.c_str(), "rb");
    if (!probe) {
        if (errno == ENOENT) return {};
        throw std::runtime_error("Cannot open data file");
    }
    std::fclose(probe);
    std::ifstream input(path_);
    if (!input) throw std::runtime_error("Cannot open data file");

    std::string header;
    if (!std::getline(input, header)) throw std::runtime_error("Invalid data file header");
    std::istringstream header_stream(header);
    std::string magic;
    CatalogSnapshot snapshot;
    if (!(header_stream >> magic >> snapshot.next_id) || magic != "LIBRARY_V1" ||
        snapshot.next_id <= 0) {
        throw std::runtime_error("Invalid data file header");
    }
    header_stream >> std::ws;
    if (!header_stream.eof()) throw std::runtime_error("Invalid data file header");

    std::set<int> ids;
    std::string line;
    int line_number = 1;
    while (std::getline(input, line)) {
        ++line_number;
        if (line.empty()) continue;
        std::istringstream row(line);
        int id, year, total, available;
        std::string title, author;
        if (!(row >> id >> std::quoted(title) >> std::quoted(author)
                  >> year >> total >> available)) {
            throw std::runtime_error("Invalid record at line " + std::to_string(line_number));
        }
        row >> std::ws;
        if (!row.eof() || !ids.insert(id).second || id >= snapshot.next_id) {
            throw std::runtime_error("Invalid or duplicate ID at line " + std::to_string(line_number));
        }
        try {
            snapshot.books.emplace_back(id, std::move(title), std::move(author),
                                        year, total, available);
        } catch (const std::invalid_argument&) {
            throw std::runtime_error("Invalid book at line " + std::to_string(line_number));
        }
    }
    if (input.bad()) throw std::runtime_error("Error reading data file");
    return snapshot;
}

void TextFileStorage::save(const CatalogSnapshot& snapshot) const {
    std::ofstream output(path_, std::ios::trunc);
    if (!output) throw std::runtime_error("Cannot write data file");
    output << "LIBRARY_V1 " << snapshot.next_id << '\n';
    for (const Book& book : snapshot.books) {
        output << book.id() << ' ' << std::quoted(book.title()) << ' '
               << std::quoted(book.author()) << ' ' << book.year() << ' '
               << book.total_copies() << ' ' << book.available_copies() << '\n';
    }
    output.close();
    if (!output) throw std::runtime_error("Error writing data file");
}

}  // namespace library
