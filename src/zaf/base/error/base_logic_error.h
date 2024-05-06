#pragma once

#include <stdexcept>
#include <zaf/base/error/error.h>

namespace zaf {

class BaseLogicError : public std::logic_error, public Error {
public:
    BaseLogicError() : logic_error("") {

    }

    explicit BaseLogicError(const std::string& message) : logic_error(message) {

    }

    explicit BaseLogicError(const SourceSite& site) : logic_error(""), Error(site) {

    }

    BaseLogicError(const std::string& message, const SourceSite& site) :
        logic_error(""), 
        Error(site) {

    }
};

}