#include <zaf/resource/uri_loader.h>
#include <zaf/application.h>
#include <zaf/base/error/system_error.h>
#include <zaf/resource/internal/resource_loader.h>
#include <zaf/resource/internal/resource_location_parsing.h>
#include <zaf/resource/internal/uri_parsing.h>

namespace zaf {
namespace {

class DefaultURILoader : public UriLoader {
public:
    Stream Load(const std::wstring& uri) override {
    
        auto parse_result = internal::ParseURI(uri);

        if (parse_result.type == internal::URIType::File) {
            return Stream::FromFile(parse_result.value);
        }

        if (parse_result.type == internal::URIType::Resource) {
            return LoadResourceURI(parse_result.value);
        }

        if (parse_result.type == internal::URIType::Relative) {
            return LoadRelativeURI(parse_result.value);
        }

        ZAF_THROW_SYSTEM_ERROR(ERROR_INVALID_PARAMETER);
    }

private:
    Stream LoadResourceURI(const std::wstring& resource_location) {

        auto location = internal::ParseResourceLocation(resource_location);
        return resource_loader_.Load(location.dll, location.name);
    }

    Stream LoadRelativeURI(const std::wstring& uri) {

        auto working_directory_path = Application::Instance().GetWorkingDirectoryPath();
        return Stream::FromFile(working_directory_path / uri);
    }

private:
    internal::ResoureLoader resource_loader_;
};

}

std::shared_ptr<UriLoader> UriLoader::GetDefaultLoader() {

    static auto instance = std::make_shared<DefaultURILoader>();
    return instance;
}

}