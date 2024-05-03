#pragma once

#include <zaf/base/error/runtime_error.h>

namespace zaf {

class NotSupportedError : public std::runtime_error, public RuntimeError {
public:
    NotSupportedError() : runtime_error("") {

    }

    explicit NotSupportedError(const SourceSite& site) : runtime_error(""), RuntimeError(site) {

    }

    const char* Message() const noexcept override {
        return this->what();
    }
};

}