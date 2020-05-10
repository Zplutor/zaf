#pragma once

#include <zaf/base/stream.h>

namespace zaf {

class UriLoader;

class ResourceManager {
public:
    ResourceManager();
    ~ResourceManager();

    ResourceManager(const ResourceManager&) = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;

    Stream LoadUri(const std::wstring& uri, std::error_code& error_code);
    Stream LoadUri(const std::wstring& uri) {
        std::error_code error_code;
        auto result = LoadUri(uri, error_code);
        return result;
    }

private:
    Stream LoadRelativeUri(const std::wstring& uri, std::error_code& error_code);

private:
    std::shared_ptr<UriLoader> relative_uri_loader_;
};

}