#pragma once

#include "book.hpp"

#include <string>
#include <vector>

namespace library {

struct CatalogSnapshot {
    int next_id = 1;
    std::vector<Book> books;
};

// 抽象接口使业务逻辑不依赖具体文件格式。
class IStorage {
public:
    virtual ~IStorage() = default;
    virtual CatalogSnapshot load() const = 0;
    virtual void save(const CatalogSnapshot& snapshot) const = 0;
};

class TextFileStorage final : public IStorage {
public:
    explicit TextFileStorage(std::string path);
    CatalogSnapshot load() const override;
    void save(const CatalogSnapshot& snapshot) const override;

private:
    std::string path_;
};

}  // namespace library
