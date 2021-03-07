#include <zaf/resource/resource_manager.h>
#include <zaf/resource/uri_loader.h>

namespace zaf {

ResourceManager::ResourceManager(const std::shared_ptr<UriLoader>& custom_uri_loader) :
    custom_uri_loader_(custom_uri_loader) {

}


ResourceManager::~ResourceManager() {

}


Stream ResourceManager::LoadUri(const std::wstring& uri) {

    if (custom_uri_loader_) {
        return custom_uri_loader_->Load(uri);
    }

    return UriLoader::GetDefaultLoader()->Load(uri);
}

}