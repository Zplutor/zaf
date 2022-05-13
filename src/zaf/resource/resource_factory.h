#pragma once

#include <zaf/base/stream.h>

namespace zaf {

class Application;
class URILoader;

class ResourceFactory {
public:
    static ResourceFactory& Instance();

public:
    ~ResourceFactory();

    ResourceFactory(const ResourceFactory&) = delete;
    ResourceFactory& operator=(const ResourceFactory&) = delete;

    Stream LoadURI(const std::wstring& uri);
    Stream LoadURI(const std::wstring& uri, float dpi);

private:
    friend class zaf::Application;

    explicit ResourceFactory(const std::shared_ptr<URILoader>& custom_uri_loader);

private:
    std::shared_ptr<URILoader> custom_uri_loader_;
};

}