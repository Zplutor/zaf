#pragma once

#include <zaf/resource/uri_loader.h>

namespace zaf {

class DefaultRelativeUriLoader : public UriLoader {
public:
    static std::shared_ptr<DefaultRelativeUriLoader> GetInstance();

public:
    Stream Load(const std::wstring& uri) override;

private:
    DefaultRelativeUriLoader() = default;
};

}