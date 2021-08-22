#pragma once

#include <Windows.h>
#include <system_error>
#include <zaf/base/error/error.h>

namespace zaf {

const std::error_category& COMCategory();

inline std::error_code MakeCOMErrorCode(HRESULT com_error_code) {
    return std::error_code{ static_cast<int>(com_error_code), COMCategory() };
}


#define ZAF_THROW_IF_COM_ERROR(hresult)                                     \
if (FAILED(hresult)) {                                                      \
    throw zaf::Error{ zaf::MakeCOMErrorCode(hresult), ZAF_SOURCE_SITE() };  \
}

}