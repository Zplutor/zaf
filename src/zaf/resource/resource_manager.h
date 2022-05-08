#pragma once

#include <zaf/base/stream.h>

namespace zaf {

class URILoader;

class ResourceManager {
public:
    ResourceManager(const std::shared_ptr<URILoader>& custom_uri_loader);
    ~ResourceManager();

    ResourceManager(const ResourceManager&) = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;

    Stream LoadURI(const std::wstring& uri);
    Stream LoadURI(const std::wstring& uri, float dpi);

private:
    std::shared_ptr<URILoader> custom_uri_loader_;
};

}