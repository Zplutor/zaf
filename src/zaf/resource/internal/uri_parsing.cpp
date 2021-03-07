#include <zaf/resource/internal/uri_parsing.h>

namespace zaf {
namespace internal {
namespace {

bool TryToParseFileURI(const std::wstring& uri, URIParseResult& result) {

    const std::wstring prefix{ L"file://" };
    if (uri.find(prefix) != 0) {
        return false;
    }

    //A formal file uri format is "file://host/path", host is unsupported here,
    //so there must be three '/'s.
    if (uri[prefix.length()] != L'/') {
        result.type = URIType::Unknown;
        return true;
    }

    std::filesystem::path path = uri.substr(prefix.length() + 1);
    //Path must be absolute.
    if (path.is_absolute()) {
        result.type = URIType::File;
        result.value = path;
    }
    else {
        result.type = URIType::Unknown;
    }
    
    return true;
}


bool TryToParseResourceURI(const std::wstring& uri, URIParseResult& result) {

    const std::wstring prefix{ L"res://" };
    if (uri.find(prefix) != 0) {
        return false;
    }

    auto location = uri.substr(prefix.length());
    if (!location.empty()) {

        result.type = URIType::Resource;
        result.value = std::move(location);
    }
    else {

        result.type = URIType::Unknown;
    }
    
    return true;
}


void ParseRelativeURI(const std::wstring& uri, URIParseResult& result) {

    if (uri.empty()) {
        result.type = URIType::Unknown;
        return;
    }

    //Implicitly convert relative uri to file path.
    std::filesystem::path path = uri;
    if (path.is_absolute()) {

        result.type = URIType::File;
        result.value = path;
    }
    else {

        result.type = URIType::Relative;
        result.value = uri;
    }
}

}

URIParseResult ParseURI(const std::wstring& uri) {

    URIParseResult result;
    if (TryToParseFileURI(uri, result)) {
        return result;
    }

    if (TryToParseResourceURI(uri, result)) {
        return result;
    }

    ParseRelativeURI(uri, result);
    return result;
}

}
}