#include <zaf/control/rich_edit/internal/data_format_enumerator.h>

namespace zaf::rich_edit::internal {

HRESULT DataFormatEnumerator::QueryInterface(REFIID riid, LPVOID* ppvObj) {

    if (!ppvObj) {
        return E_INVALIDARG;
    }

    if (riid == IID_IUnknown || riid == IID_IEnumFORMATETC) {
        *ppvObj = this;
        AddRef();
        return S_OK;
    }

    *ppvObj = nullptr;
    return E_NOINTERFACE;
}


ULONG DataFormatEnumerator::AddRef() {
    return InterlockedIncrement(&reference_count_);
}


ULONG DataFormatEnumerator::Release() {
    ULONG result = InterlockedDecrement(&reference_count_);
    if (result == 0) {
        delete this;
    }
    return result;
}


HRESULT DataFormatEnumerator::Next(ULONG celt, FORMATETC* rgelt, ULONG* pceltFetched) {

    if (index_ < count_) {
        rgelt->cfFormat = CF_UNICODETEXT;
        rgelt->ptd = nullptr;
        rgelt->dwAspect = DVASPECT_CONTENT;
        rgelt->lindex = -1;
        rgelt->tymed = TYMED_HGLOBAL;

        ++index_;
        return S_OK;
    }
    return S_FALSE;
}


HRESULT DataFormatEnumerator::Skip(ULONG celt) {
    return E_NOTIMPL;
}


HRESULT DataFormatEnumerator::Reset(void) {
    count_ = 2;
    index_ = 0;
    return S_OK;
}


HRESULT DataFormatEnumerator::Clone(IEnumFORMATETC** ppenum) {
    return E_NOTIMPL;
}

}