#pragma once

#include <zaf/base/error/base_system_error.h>

namespace zaf {

class NTError : public BaseSystemError {
public:
    static const std::error_category& Category();

public:
    NTError(NTSTATUS code, const SourceSite& site) :
        BaseSystemError(std::error_code(static_cast<int>(code), Category()), site) {

    }
};

}