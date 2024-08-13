#include <zaf/resource/internal/uri_parsing.h>

namespace zaf {
namespace internal {
namespace {

bool TryToParseFileURI(std::wstring_view uri, URIParseResult& result) {

    std::wstring_view prefix{ L"file://" };
    if (uri.find(prefix) != 0) {
        return false;
    }

    //A formal file uri format is "file://host/path", host is unsupported here,
    //so there must be three '/'s.
    if (uri.length() <= prefix.length() || uri[prefix.length()] != L'/') {
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


bool TryToParseResourceURI(std::wstring_view uri, URIParseResult& result) {

    std::wstring_view prefix{ L"res://" };
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


void ParseRelativeURI(std::wstring_view uri, URIParseResult& result) {

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

URIParseResult ParseURI(std::wstring_view uri) {

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