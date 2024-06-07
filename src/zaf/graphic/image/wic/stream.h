#pragma once

#include <wincodec.h>
#include <zaf/base/com_object.h>
#include <zaf/base/error/com_error.h>
#include <zaf/io/stream/stream.h>

namespace zaf::wic {

class Stream : public COMObject<IWICStream> {
public:
    using COMObject::COMObject;

    void InitializeFromMemory(void* data, std::size_t size) {
        HRESULT com_error = Ptr()->InitializeFromMemory(
            reinterpret_cast<BYTE*>(data),
            static_cast<DWORD>(size));
        ZAF_THROW_IF_COM_ERROR(com_error);
    }

    void InitializeFromStream(const zaf::Stream& stream) {
        HRESULT com_error = Ptr()->InitializeFromIStream(stream.Ptr().Inner());
        ZAF_THROW_IF_COM_ERROR(com_error);
    }
};

}