#pragma once

#include <wincodec.h>
#include <zaf/base/com_ptr.h>
#include <zaf/base/error/com_error.h>
#include <zaf/base/stream.h>

namespace zaf::wic {

class Stream : public COMPtr<IWICStream> {
public:
    Stream() { }
    explicit Stream(IWICStream* handle) : COMPtr(handle) { }

    void InitializeFromMemory(void* data, std::size_t size) {
        HRESULT com_error = Inner()->InitializeFromMemory(
            reinterpret_cast<BYTE*>(data),
            static_cast<DWORD>(size));
        ZAF_THROW_IF_COM_ERROR(com_error);
    }

    void InitializeFromStream(const zaf::Stream& stream) {
        HRESULT com_error = Inner()->InitializeFromIStream(stream.Inner());
        ZAF_THROW_IF_COM_ERROR(com_error);
    }
};

}