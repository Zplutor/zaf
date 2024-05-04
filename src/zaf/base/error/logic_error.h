#pragma once

#include <zaf/base/error/error.h>

namespace zaf {

/**
Base class for all logic errors in the zaf library.
*/
class LogicError : public Error {
public:
    using Error::Error;
};


class GeneralLogicError : public std::logic_error, public LogicError {
public:
    explicit GeneralLogicError(const SourceSite& site) : logic_error(""), LogicError(site) {

    }

    GeneralLogicError(const std::string& message, const SourceSite& site) :
        logic_error(message),
        LogicError(site) {

    }

    const char* Message() const noexcept override {
        return this->what();
    }
};

}