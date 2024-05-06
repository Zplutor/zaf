#pragma once

#include <stdexcept>
#include <zaf/base/error/error.h>

namespace zaf {

class BaseRuntimeError : public std::runtime_error, public Error {
public:
    BaseRuntimeError() : runtime_error("") {

    }

    explicit BaseRuntimeError(const std::string& message) : runtime_error(message) {

    }

    explicit BaseRuntimeError(const SourceSite& site) : runtime_error(""), Error(site) {

    }

    BaseRuntimeError(const std::string& message, const SourceSite& site) :
        runtime_error(""),
        Error(site) {

    }
};


class GeneralRuntimeError : public BaseRuntimeError {
public:
    BaseRuntimeError::BaseRuntimeError;
};

}