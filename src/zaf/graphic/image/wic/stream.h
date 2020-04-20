#pragma once

#include <wincodec.h>
#include <zaf/base/com_object.h>
#include <zaf/base/error.h>

namespace zaf::wic {

class Stream : public ComObject<IWICStream> {
public:
    Stream() { }
    explicit Stream(IWICStream* handle) : ComObject(handle) { }

    void Initialize(void* data, std::size_t size, std::error_code& error_code) {
        HRESULT com_error = GetHandle()->InitializeFromMemory(reinterpret_cast<BYTE*>(data), size);
        error_code = MakeComErrorCode(com_error);
    }

    void Initialize(void* data, std::size_t size) {
        std::error_code error_code;
        Initialize(data, size, error_code);
        ZAF_CHECK_ERROR(error_code);
    }
};

}