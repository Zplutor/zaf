#include <zaf/resource/internal/uri_parsing.h>

namespace zaf {
namespace internal {

UriParseResult ParseUri(const std::wstring& uri) {

    UriParseResult result;

    if (uri.empty()) {
        result.type = UriType::Unknown;
        return result;
    }

    const std::wstring file_schema_prefix{ L"file:///" };
    if (uri.find(file_schema_prefix) == 0) {
        
        std::filesystem::path path = uri.substr(file_schema_prefix.length());
        if (path.is_absolute()) {
            result.type = UriType::File;
            result.value = path;
            return result;
        }

        result.type = UriType::Unknown;
        return result;
    }

    //Implicitly convert to file path.
    std::filesystem::path path = uri;
    if (path.is_absolute()) {
        result.type = UriType::File;
        result.value = path;
        return result;
    }

    result.type = UriType::Relative;
    result.value = uri;
    return result;
}

}
}