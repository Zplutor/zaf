#pragma once

#include <zaf/base/source_site.h>

namespace zaf {

/**
Base class for all exceptions in the zaf library.
*/
class Error {
public:
    explicit Error(const SourceSite& site) : site_(site) { }
    virtual ~Error() = default;

    const SourceSite& Site() const noexcept {
        return site_;
    }

    virtual const char* Message() const noexcept = 0;

private:
    SourceSite site_;
};

}