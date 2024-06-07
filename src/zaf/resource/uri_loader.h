#pragma once

#include <zaf/io/stream/stream.h>

namespace zaf {

class URILoader {
public:
    static std::shared_ptr<URILoader> DefaultLoader();

public:
    URILoader() = default;
    virtual ~URILoader() = default;

    URILoader(const URILoader&) = delete;
    URILoader& operator=(const URILoader&) = delete;

    virtual Stream Load(const std::wstring& uri, float dpi) = 0;
};

}