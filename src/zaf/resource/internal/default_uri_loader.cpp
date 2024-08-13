#include <zaf/resource/internal/default_uri_loader.h>
#include <zaf/application.h>
#include <zaf/resource/internal/resource_location_parsing.h>
#include <zaf/resource/internal/uri_parsing.h>
#include <zaf/resource/invalid_uri_error.h>

namespace zaf::internal {

Stream DefaultURILoader::Load(std::wstring_view uri, float dpi) {

    auto parse_result = internal::ParseURI(uri);

    if (parse_result.type == internal::URIType::File) {
        return Stream::FromFile(parse_result.value);
    }

    if (parse_result.type == internal::URIType::Resource) {
        return LoadResourceURI(parse_result.value, dpi);
    }

    if (parse_result.type == internal::URIType::Relative) {
        return LoadRelativeURI(parse_result.value);
    }

    throw InvalidURIError{ ZAF_SOURCE_LOCATION() };
}


Stream DefaultURILoader::LoadResourceURI(std::wstring_view resource_location, float dpi) {

    auto location = internal::ParseResourceLocation(resource_location);
    return resource_loader_.Load(location.dll, location.name, dpi);
}


Stream DefaultURILoader::LoadRelativeURI(std::wstring_view uri) {

    auto working_directory_path = Application::Instance().GetWorkingDirectoryPath();
    return Stream::FromFile(working_directory_path / uri);
}

}