#include <zaf/resource/resource_manager.h>
#include <zaf/application.h>
#include <zaf/resource/uri_loader.h>

namespace zaf {

ResourceManager& ResourceManager::Instance() {
    return Application::Instance().GetResourceManager();
}


ResourceManager::ResourceManager(const std::shared_ptr<URILoader>& custom_uri_loader) :
    custom_uri_loader_(custom_uri_loader) {

}


ResourceManager::~ResourceManager() {

}


Stream ResourceManager::LoadURI(const std::wstring& uri) {
    return LoadURI(uri, Application::Instance().GetSystemDPI());
}


Stream ResourceManager::LoadURI(const std::wstring& uri, float dpi) {

    if (custom_uri_loader_) {
        return custom_uri_loader_->Load(uri, dpi);
    }

    return URILoader::DefaultLoader()->Load(uri, dpi);
}

}