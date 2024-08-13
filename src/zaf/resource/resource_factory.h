#pragma once

#include <zaf/io/stream/stream.h>

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

    Stream LoadURI(std::wstring_view uri);
    Stream LoadURI(std::wstring_view, float dpi);

private:
    friend class zaf::Application;

    explicit ResourceFactory(const std::shared_ptr<URILoader>& custom_uri_loader);

private:
    std::shared_ptr<URILoader> custom_uri_loader_;
};

}