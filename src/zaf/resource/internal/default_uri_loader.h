#pragma once

#include <zaf/resource/internal/resource_loader.h>
#include <zaf/resource/uri_loader.h>

namespace zaf::internal {

class DefaultURILoader : public URILoader {
public:
    Stream Load(const std::wstring& uri, float dpi) override;

private:
    Stream LoadResourceURI(const std::wstring& resource_location, float dpi);
    Stream LoadRelativeURI(const std::wstring& uri);

private:
    internal::ResourceLoader resource_loader_;
};

}