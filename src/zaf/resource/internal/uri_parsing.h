#pragma once

#include <filesystem>

namespace zaf {
namespace internal {

enum class UriType {
    Unknown,
    File,
    Relative,
};

class UriParseResult {
public:
    UriType type{ UriType::Unknown };
    std::wstring value;
};

UriParseResult ParseUri(const std::wstring& uri);

}
}