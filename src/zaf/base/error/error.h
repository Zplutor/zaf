#pragma once

#include <zaf/base/source_site.h>

namespace zaf {

/**
 Contains information abount a runtime error.
 */
class Error {
public:
    Error() = default;

    explicit Error(const std::error_code& code) : code_(code) { }

    Error(const std::error_code& code, const SourceSite& site) : code_(code), site_(site) { }

    Error(const std::error_code& code, const std::string& message) : 
        code_(code), 
        message_(message) {

    }

    Error(const std::error_code& code, const std::string& message, const SourceSite& site) :
        code_(code),
        message_(message),
        site_(site) {

    }

    const std::error_code& Code() const {
        return code_;
    }

    const std::string& Message() const {
        return message_;
    }

    const SourceSite& Site() const {
        return site_;
    }

private:
    std::string message_;
    std::error_code code_;
    SourceSite site_;
};

}