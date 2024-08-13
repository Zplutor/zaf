#pragma once

#include <string_view>

namespace zaf::internal {

class ResourceLocation {
public:
    std::wstring dll;
    std::wstring name;
};

ResourceLocation ParseResourceLocation(std::wstring_view string);

}