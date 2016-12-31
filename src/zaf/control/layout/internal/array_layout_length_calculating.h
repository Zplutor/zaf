#pragma once

#include <vector>

namespace zaf {
namespace internal {

class ArrayLayoutLengthCalculatingItem {
public:
    float minimum_length;
    float maximum_length;
    float current_length;
    std::size_t round;
};

void CalculateArrayLayoutLengths(float total_length, std::vector<ArrayLayoutLengthCalculatingItem>& items);

}
}