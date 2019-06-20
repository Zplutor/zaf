#include <zaf/parsing/parsers/internal/utility.h>
#include <zaf/base/string/split.h>
#include <zaf/base/string/to_numeric.h>

namespace zaf {
namespace internal {
namespace {

bool ParseAttributeToFloatArray(const std::wstring& attribute, float* array, std::size_t count) {

    auto splitted_values = Split(attribute, L',');
    if (splitted_values.size() != count) {
        return false;
    }

    for (std::size_t index = 0; index < count; ++index) {

        if (!TryToNumeric<float>(splitted_values[index], array[index])) {
            return false;
        }
    }

    return true;
}

}

bool ParseAttributeToDoubleFloats(
    const std::wstring& attribute,
    float& float0,
    float& float1) {

    float array[2]{};
    if (! ParseAttributeToFloatArray(attribute, array, std::size(array))) {
        return false;
    }

    float0 = array[0];
    float1 = array[1];
    return true;
}


bool ParseAttributeToQuaterFloats(
    const std::wstring& attribute,
    float& float0,
    float& float1,
    float& float2,
    float& float3) {

    float array[4]{};
    if (! ParseAttributeToFloatArray(attribute, array, std::size(array))) {
        return false;
    }

    float0 = array[0];
    float1 = array[1];
    float2 = array[2];
    float3 = array[3];
    return true;
}

}
}