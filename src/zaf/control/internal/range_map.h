#pragma once

#include <zaf/control/internal/range_manager.h>

namespace zaf {
namespace internal {

template<typename ValueType>
class RangeMap {
public:
    void AddRange(const Range& range, ValueType value) {
        range_manager_.AddRange(range, std::move(value));
    }

    void RemoveRange(const Range& range) {
        range_manager_.RemoveRange(range);
    }

    void Clear() {
        range_manager_.Clear();
    }

    const ValueType* GetValueAtIndex(std::size_t index) const {

        auto value = range_manager_.GetRangeDataAtIndex(index);
        if (value) {
            return std::any_cast<ValueType>(value);
        }
        return nullptr;
    }

    std::vector<std::pair<Range, ValueType>> GetAllRangesAndValues() const {

        std::size_t range_count = range_manager_.RangeCount();

        std::vector<std::pair<Range, ValueType>> ranges_and_values;
        ranges_and_values.reserve(range_count);

        for (std::size_t index = 0; index < range_count; ++index) {

            const auto& range = range_manager_.GetRange(index);
            const auto& data = range_manager_.GetRangeData(index);

            ranges_and_values.push_back(std::make_pair(range, std::any_cast<ValueType>(data)));
        }

        return ranges_and_values;
    }

private:
    RangeManager range_manager_;
};

}
}