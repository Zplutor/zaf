#pragma once

#include <stdexcept>
#include <zaf/base/error/error.h>

namespace zaf {

/**
Base class for all runtime errors in the zaf library.
*/
class RuntimeError : public Error {
public:
    using Error::Error;
};


class GeneralRuntimeError : public std::runtime_error, public RuntimeError {
public:
    explicit GeneralRuntimeError(const SourceSite& site) : runtime_error(""), RuntimeError(site) {

    }

    GeneralRuntimeError(const std::string& message, const SourceSite& site) : 
        runtime_error(message),
        RuntimeError(site) {

    }

    const char* Message() const noexcept override {
        return this->what();
    }
};

}