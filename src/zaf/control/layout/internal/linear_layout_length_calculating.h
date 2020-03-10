#pragma once

#include <vector>

namespace zaf {
namespace internal {

class LinearLayoutLengthCalculatItem {
public:
    float minimum_length{};
    float maximum_length{};
    float current_length{};
};

void CalculateArrayLayoutLengths(
    float total_length,
    std::vector<LinearLayoutLengthCalculatItem>& items);

}
}