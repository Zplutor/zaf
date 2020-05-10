#pragma once

#include <zaf/base/stream.h>

namespace zaf {

class UriLoader {
public:
    UriLoader() = default;
    virtual ~UriLoader() = default;

    UriLoader(const UriLoader&) = delete;
    UriLoader& operator=(const UriLoader&) = delete;

    virtual Stream Load(const std::wstring& uri, std::error_code& error_code) = 0;
};

}