#pragma once

#include <wincodec.h>
#include <zaf/base/com_object.h>
#include <zaf/base/error.h>
#include <zaf/base/stream.h>

namespace zaf::wic {

class Stream : public ComObject<IWICStream> {
public:
    Stream() { }
    explicit Stream(IWICStream* handle) : ComObject(handle) { }

    void InitializeFromMemory(void* data, std::size_t size, std::error_code& error_code) {
        HRESULT com_error = GetHandle()->InitializeFromMemory(reinterpret_cast<BYTE*>(data), size);
        error_code = MakeComErrorCode(com_error);
    }

    void InitializeFromMemory(void* data, std::size_t size) {
        std::error_code error_code;
        InitializeFromMemory(data, size, error_code);
        ZAF_CHECK_ERROR(error_code);
    }

    void InitializeFromStream(const zaf::Stream& stream, std::error_code& error_code) {
        HRESULT com_error = GetHandle()->InitializeFromIStream(stream.GetHandle());
        error_code = MakeComErrorCode(com_error);
    }

    void InitializeFromStream(const zaf::Stream& stream) {
        std::error_code error_code;
        InitializeFromStream(stream, error_code);
        ZAF_CHECK_ERROR(error_code);
    }
};

}