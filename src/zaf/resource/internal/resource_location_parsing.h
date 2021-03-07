#pragma once

namespace zaf::internal {

class ResourceLocation {
public:
    std::wstring dll;
    std::wstring name;
};

ResourceLocation ParseResourceLocation(const std::wstring& string);

}