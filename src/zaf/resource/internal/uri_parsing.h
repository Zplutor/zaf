#pragma once

#include <filesystem>

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

URIParseResult ParseURI(const std::wstring& uri);

}
}