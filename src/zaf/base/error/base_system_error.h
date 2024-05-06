#pragma once

#include <system_error>
#include <zaf/base/error/error.h>

namespace zaf {

class BaseSystemError : public std::system_error, public Error {
public:
    BaseSystemError(const std::error_code& error_code, const SourceSite& site) :
        system_error(error_code),
        Error(site) {

    }

    BaseSystemError(
        const std::error_code& error_code,
        const std::string& message,
        const SourceSite& site) 
        :
        system_error(error_code, message),
        Error(site) {

    }
};

}