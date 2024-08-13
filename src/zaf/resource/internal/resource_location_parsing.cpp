#include <zaf/resource/internal/resource_location_parsing.h>

namespace zaf::internal {

ResourceLocation ParseResourceLocation(std::wstring_view string) {

    ResourceLocation result;

    auto delimiter_index = string.find(L'/');
    if (delimiter_index == std::wstring::npos) {
        return result;
    }

    result.dll = string.substr(0, delimiter_index);
    result.name = string.substr(delimiter_index + 1);
    return result;
}

}