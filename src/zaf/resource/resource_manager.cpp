#include <zaf/resource/resource_manager.h>
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


Stream ResourceManager::LoadUri(const std::wstring& uri, std::error_code& error_code) {

    auto uri_parse_result = internal::ParseUri(uri);

    if (uri_parse_result.type == internal::UriType::File) {
        return Stream::FromFile(uri_parse_result.value, error_code);
    }

    if (uri_parse_result.type == internal::UriType::Relative) {
        return LoadRelativeUri(uri_parse_result.value, error_code);
    }

    error_code = MakeSystemErrorCode(ERROR_INVALID_PARAMETER);
    return {};
}


Stream ResourceManager::LoadRelativeUri(const std::wstring& uri, std::error_code& error_code) {

    if (relative_uri_loader_) {
        return relative_uri_loader_->Load(uri, error_code);
    }
    
    return DefaultRelativeUriLoader::GetInstance()->Load(uri, error_code);
}

}