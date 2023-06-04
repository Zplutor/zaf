#pragma once

#include <ObjIdl.h>
#include <zaf/base/non_copyable.h>

namespace zaf::clipboard::internal {

class MediumWrapper : public IUnknown, NonCopyableNonMovable {
public:
    explicit MediumWrapper(const STGMEDIUM& medium) : wrapped_medium_(medium) {

    }

    ~MediumWrapper() {
        ReleaseStgMedium(&wrapped_medium_);
    }

    HRESULT QueryInterface(REFIID riid, LPVOID* ppvObj) override {

        if (!ppvObj) {
            return E_INVALIDARG;
        }

        if (riid == IID_IUnknown) {
            *ppvObj = this;
            AddRef();
            return S_OK;
        }

        *ppvObj = nullptr;
        return E_NOINTERFACE;
    }


    ULONG AddRef() override {
        return InterlockedIncrement(&reference_count_);
    }


    ULONG Release() override {
        ULONG result = InterlockedDecrement(&reference_count_);
        if (result == 0) {
            delete this;
        }
        return result;
    }

private:
    LONG reference_count_{ 1 };
    STGMEDIUM wrapped_medium_{};
};

}