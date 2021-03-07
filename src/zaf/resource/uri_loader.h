#pragma once

#include <zaf/base/stream.h>

namespace zaf {

class UriLoader {
public:
    static std::shared_ptr<UriLoader> GetDefaultLoader();

public:
    UriLoader() = default;
    virtual ~UriLoader() = default;

    UriLoader(const UriLoader&) = delete;
    UriLoader& operator=(const UriLoader&) = delete;

    virtual Stream Load(const std::wstring& uri) = 0;
};

}