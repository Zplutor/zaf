#pragma once

#include <map>
#include <vector>

namespace zaf {
namespace internal {

class RangeManager {
public:
    void AddRange(std::size_t position, std::size_t length);

private:
    std::vector<std::pair<std::size_t, std::size_t>> ranges_;
};

template<typename ValueType>
class RangeMap {
public:
    struct Range {
        std::size_t position;
        std::size_t length;
    };

public:
    void AddValueAtRange(const ValueType& value, std::size_t position, std::size_t length) {

        std::size_t need_added_range_index = index;
        std::size_t need_added_range_end_index = index + count;

        auto iterator = selections_.begin();
        while (iterator != selections_.end()) {

            std::size_t current_range_index = iterator->first;
            std::size_t current_range_end_index = iterator->first + iterator->second;

            if (need_added_range_index < current_range_end_index) {

                if (need_added_range_end_index <= current_range_index) {

                    std::size_t add_count = need_added_range_end_index - need_added_range_index;
                    iterator = selections_.insert(iterator, std::make_pair(need_added_range_index, add_count));
                }
                else if (need_added_range_index < current_range_index) {

                    std::size_t add_count = current_range_index - need_added_range_index;
                    iterator = selections_.insert(iterator, std::make_pair(need_added_range_index, add_count));
                }

                need_added_range_index = current_range_end_index;

                if (need_added_range_index >= need_added_range_end_index) {
                    return;
                }
            }

            ++iterator;
        }

        selections_.insert(
            iterator,
            std::make_pair(need_added_range_index, need_added_range_end_index - need_added_range_index));
    }

private:
    std::vector<Range> ranges_;
    std::map<std::size_t, ValueType> values_;
};

}
}