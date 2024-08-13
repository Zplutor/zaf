#pragma once

#include <zaf/resource/internal/resource_loader.h>
#include <zaf/resource/uri_loader.h>

namespace zaf::internal {

class DefaultURILoader : public URILoader {
public:
    Stream Load(std::wstring_view uri, float dpi) override;

private:
    Stream LoadResourceURI(std::wstring_view resource_location, float dpi);
    Stream LoadRelativeURI(std::wstring_view uri);

private:
    internal::ResourceLoader resource_loader_;
};

}