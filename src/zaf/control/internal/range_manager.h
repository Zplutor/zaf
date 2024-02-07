#pragma once

#include <any>
#include <optional>
#include <vector>
#include <zaf/base/range.h>

namespace zaf {
namespace internal {

class RangeManager {
public:
    class Item {
    public:
        Range range;
        std::any value;
    };

    using ItemList = std::vector<Item>;

public:
    bool AddRange(const Range& new_range, std::any value);
    bool AddRange(std::size_t index, std::size_t length);

    bool RemoveRange(const Range& removed_range);
    bool RemoveRange(std::size_t index, std::size_t length);

    bool InsertSpan(const Range& span_range);
    bool EraseSpan(const Range& span_range);

    void Clear();

    const Item* FindItemContainsIndex(std::size_t index) const;

    const ItemList& Items() const {
        return items_;
    }

private:
    bool ReplaceRange(const Range& replaced_range, std::optional<std::any> value);

private:
    ItemList items_;
};

}
}