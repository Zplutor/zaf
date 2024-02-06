#pragma once

#include <any>
#include <functional>
#include <optional>
#include <vector>
#include <zaf/base/non_copyable.h>
#include <zaf/base/range.h>

namespace zaf {
namespace internal {

class RangeManager : NonCopyableNonMovable {
public:
    RangeManager() = default;

    bool AddRange(const Range& new_range, std::any data);
    bool AddRange(const Range& new_range);
    bool AddRange(std::size_t index, std::size_t length);

    bool RemoveRange(const Range& removed_range);
    bool RemoveRange(std::size_t index, std::size_t length);

    bool InsertSpan(const Range& span_range);
    bool EraseSpan(const Range& span_range);
    
    void Clear();

    std::size_t RangeCount() const {
        return items_.size();
    }

    const Range& GetRange(std::size_t range_index) const;
    const std::any& GetRangeData(std::size_t range_index) const;

    const std::any* GetRangeDataAtIndex(std::size_t index) const;

    bool IsIndexInRange(std::size_t index) const;

private:
    class Item {
    public:
        Range range;
        std::any data;
    };

private:
    bool ReplaceRange(const Range& replaced_range, std::optional<std::any> data);
    const Item* GetItemAtIndex(std::size_t index) const;

private:
    std::vector<Item> items_;
};

}
}