#include <zaf/control/layout/internal/array_layout_length_calculating.h>
#include <algorithm>

namespace zaf {
namespace internal {

void CalculateArrayLayoutLengths(float total_length, std::vector<ArrayLayoutLengthCalculatingItem>& items) {

    //Initialize fields
    for (auto& each_item : items) {
        each_item.round = 0;
        each_item.current_length = (each_item.minimum_length + each_item.maximum_length) / 2;
    }

    float need_allocated_length = total_length;
    std::size_t need_allocated_count = items.size();

    std::size_t current_round = 1;

    std::size_t index = 0;
    while (index < items.size()) {

        auto& item = items[index];

        //No need to process item at the same round.
        if (item.round == current_round) {
            index++;
            continue;
        }

        item.round = current_round;

        //Calculate expected length and actual length
        float expected_length = need_allocated_length / need_allocated_count;
        float actual_length = std::max(
            std::min(expected_length, item.maximum_length),
            item.minimum_length);

        //Expected length is not equal to actual length, need further check.
        if (actual_length != expected_length) {

            //Current length is equal to actual length, no need to update this
            //item, continue next one.
            if (item.current_length == actual_length) {

                need_allocated_length -= actual_length;
                need_allocated_count--;

                index++;
            }
            //Current length is not equal to actual length, need to update this
            //item and re-calcuate all previous items.
            else {

                item.current_length = actual_length;

                need_allocated_length = total_length - actual_length;
                need_allocated_count = items.size() - 1;

                current_round++;
                item.round = current_round;

                index = 0;
            }
        }
        //Expected length is equal to actual length, just update this item.
        else {

            item.current_length = actual_length;

            need_allocated_length -= actual_length;
            need_allocated_count--;

            index++;
        }
    }
}

}
}