#pragma once

#include <zaf/base/error/base_system_error.h>

namespace zaf {

class NTError : public BaseSystemError {
public:
    static const std::error_category& Category();

public:
    explicit NTError(NTSTATUS error_value) :
        BaseSystemError(static_cast<int>(error_value), Category()) {

    }

    NTError(NTSTATUS error_value, const SourceSite& site) :
        BaseSystemError(static_cast<int>(error_value), Category(), site) {

    }
};

}