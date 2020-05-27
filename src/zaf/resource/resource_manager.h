#pragma once

#include <zaf/base/stream.h>

namespace zaf {

class UriLoader;

class ResourceManager {
public:
    ResourceManager(const std::shared_ptr<UriLoader>& relative_uri_loader);
    ~ResourceManager();

    ResourceManager(const ResourceManager&) = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;

    Stream LoadUri(const std::wstring& uri);

private:
    Stream LoadRelativeUri(const std::wstring& uri);

private:
    std::shared_ptr<UriLoader> relative_uri_loader_;
};

}