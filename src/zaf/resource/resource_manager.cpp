#include <zaf/resource/resource_manager.h>
#include <zaf/base/error/system_error.h>
#include <zaf/base/stream.h>
#include <zaf/resource/default_relative_uri_loader.h>
#include <zaf/resource/internal/uri_parsing.h>
#include <zaf/resource/uri_loader.h>

namespace zaf {

ResourceManager::ResourceManager(const std::shared_ptr<UriLoader>& relative_uri_loader) : 
    relative_uri_loader_(relative_uri_loader) {

}


ResourceManager::~ResourceManager() {

}


Stream ResourceManager::LoadUri(const std::wstring& uri) {

    auto uri_parse_result = internal::ParseUri(uri);

    if (uri_parse_result.type == internal::UriType::File) {
        return Stream::FromFile(uri_parse_result.value);
    }

    if (uri_parse_result.type == internal::UriType::Relative) {
        return LoadRelativeUri(uri_parse_result.value);
    }

    ZAF_THROW_IF_SYSTEM_ERROR(ERROR_INVALID_PARAMETER);
}


Stream ResourceManager::LoadRelativeUri(const std::wstring& uri) {

    if (relative_uri_loader_) {
        return relative_uri_loader_->Load(uri);
    }
    
    return DefaultRelativeUriLoader::GetInstance()->Load(uri);
}

}