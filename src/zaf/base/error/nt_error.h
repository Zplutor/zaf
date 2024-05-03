#pragma once

#include <zaf/base/error/system_error.h>

namespace zaf {

class NTError : public GeneralSystemError {
public:
    static const std::error_category& Category();

public:
    explicit NTError(NTSTATUS code) :
        GeneralSystemError(std::error_code(static_cast<int>(code), Category())) {

    }

    NTError(NTSTATUS code, const SourceSite& site) :
        GeneralSystemError(std::error_code(static_cast<int>(code), Category())) {

    }
};

}