#include <zaf/resource/resource_factory.h>
#include <zaf/application.h>
#include <zaf/resource/uri_loader.h>

namespace zaf {

ResourceFactory& ResourceFactory::Instance() {
    return Application::Instance().GetResourceFactory();
}


ResourceFactory::ResourceFactory(const std::shared_ptr<URILoader>& custom_uri_loader) :
    custom_uri_loader_(custom_uri_loader) {

}


ResourceFactory::~ResourceFactory() {

}


Stream ResourceFactory::LoadURI(std::wstring_view uri) {
    return LoadURI(uri, Application::Instance().GetSystemDPI());
}


Stream ResourceFactory::LoadURI(std::wstring_view uri, float dpi) {

    if (custom_uri_loader_) {
        return custom_uri_loader_->Load(uri, dpi);
    }

    return URILoader::DefaultLoader()->Load(uri, dpi);
}

}