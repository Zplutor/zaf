#pragma once

#include <zaf/base/source_site.h>

namespace zaf {

/**
Base class for all errors in the zaf library.
*/
class Error {
public:
    Error() = default;
    explicit Error(const SourceSite& site) : site_(site) { }
    virtual ~Error() = default;

    const SourceSite& Site() const {
        return site_;
    }

private:
    SourceSite site_;
};

}