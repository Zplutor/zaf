#pragma once

#include <zaf/base/stream.h>

namespace zaf {

class Application;
class URILoader;

class ResourceManager {
public:
    static ResourceManager& Instance();

public:
    ~ResourceManager();

    ResourceManager(const ResourceManager&) = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;

    Stream LoadURI(const std::wstring& uri);
    Stream LoadURI(const std::wstring& uri, float dpi);

private:
    friend class zaf::Application;

    explicit ResourceManager(const std::shared_ptr<URILoader>& custom_uri_loader);

private:
    std::shared_ptr<URILoader> custom_uri_loader_;
};

}