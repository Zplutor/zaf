#include <zaf/control/layout/internal/linear_layout_length_calculating.h>
#include <algorithm>
#include <functional>

namespace zaf {
namespace internal {
namespace {

void CheckLimit(
    float total_length,
    bool check_maximum, 
    std::vector<LinearLayoutLengthCalculatItem>& items) {

    float need_allocated_length = total_length;
    float need_allocated_count = static_cast<float>(items.size());
    float reallocated_length = 0;

    std::vector<bool> has_set_limit_items;
    has_set_limit_items.resize(items.size(), false);

    std::size_t index = 0;
    while (index < items.size()) {

        auto& item = items[index];

        float limit = check_maximum ? item.maximum_length : item.minimum_length;

        if (reallocated_length != 0) {

            if (has_set_limit_items[index]) {
                ++index;
                continue;
            }

            item.current_length = reallocated_length;
        }

        bool is_beyond_limit = check_maximum ?
            (item.current_length > item.maximum_length) :
            (
                (item.current_length < item.minimum_length) ||
                (item.current_length > item.maximum_length)
            );

        if (is_beyond_limit) {

            item.current_length = limit;
            has_set_limit_items[index] = true;

            need_allocated_length -= item.current_length;
            need_allocated_count -= 1;

            reallocated_length = need_allocated_length / need_allocated_count;
            index = 0;
        }
        else {
            ++index;
        }
    }
}

}

void CalculateArrayLayoutLengths(
    float total_length, 
    std::vector<LinearLayoutLengthCalculatItem>& items) {

    float average = total_length / items.size();
    for (auto& each_item : items) {
        each_item.current_length = average;
    }

    CheckLimit(total_length, true, items);
    CheckLimit(total_length, false, items);
}

}
}