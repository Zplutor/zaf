#include <zaf/control/layout/internal/array_layout_length_calculating.h>
#include <algorithm>
#include <functional>

namespace zaf {
namespace internal {

void CalculateArrayLayoutLengths(float total_length, std::vector<ArrayLayoutLengthCalculatingItem>& items) {

    float average = total_length / items.size();
    for (auto& each_item : items) {
        each_item.current_length = average;
    }

    auto check_limit = [total_length, &items](
        const std::function<float(const ArrayLayoutLengthCalculatingItem&)>& get_limit,
        const std::function<bool(const ArrayLayoutLengthCalculatingItem&)>& has_beyond_limit) {
    
        float need_allocated_length = total_length;
        float need_allocated_count = static_cast<float>(items.size());
        float reallocated_length = 0;

        std::size_t index = 0;
        while (index < items.size()) {

            auto& item = items[index];

            if (reallocated_length != 0) {

                if (item.current_length == get_limit(item)) {
                    ++index;
                    continue;
                }
                
                item.current_length = reallocated_length;
            }

            if (has_beyond_limit(item)) {

                item.current_length = get_limit(item);

                need_allocated_length -= item.current_length;
                need_allocated_count -= 1;

                reallocated_length = need_allocated_length / need_allocated_count;
                index = 0;
            }
            else {
                ++index;
            }
        }
    };

    check_limit(
        [](const ArrayLayoutLengthCalculatingItem& item) { return item.maximum_length; },
        [](const ArrayLayoutLengthCalculatingItem& item) { return item.current_length > item.maximum_length; }
    );

    check_limit(
        [](const ArrayLayoutLengthCalculatingItem& item) { return item.minimum_length; },
        [](const ArrayLayoutLengthCalculatingItem& item) { return item.current_length < item.minimum_length; }
    );
}

}
}