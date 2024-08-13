#pragma once

#include <filesystem>
#include <string_view>

namespace zaf {
namespace internal {

enum class URIType {
    Unknown,
    File,
    Resource,
    Relative,
};

class URIParseResult {
public:
    URIType type{ URIType::Unknown };
    std::wstring value;
};

URIParseResult ParseURI(std::wstring_view uri);

}
}