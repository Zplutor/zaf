#pragma once

#include <Windows.h>
#include <system_error>
#include <zaf/base/error/error.h>

namespace zaf {

const std::error_category& ComCategory();

inline std::error_code MakeComErrorCode(HRESULT com_error_code) {
    return std::error_code{ static_cast<int>(com_error_code), ComCategory() };
}


#define ZAF_THROW_IF_COM_ERROR(hresult)                                     \
if (FAILED(hresult)) {                                                      \
    throw zaf::Error{ zaf::MakeComErrorCode(hresult), ZAF_SOURCE_SITE() };  \
}

}