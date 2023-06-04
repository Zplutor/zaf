#include <zaf/base/clipboard/data_format_enumerator.h>

namespace zaf {

DataFormatEnumerator::DataFormatEnumerator(std::shared_ptr<internal::DataFormatItemList> items) :
    format_items_(std::move(items)) {

}


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

    if (current_index_ >= format_items_->size()) {
        return S_FALSE;
    }

    ULONG format_index{};

    auto end_index = (std::min)(current_index_ + celt, format_items_->size());
    for (; current_index_ < end_index; ++current_index_) {

        rgelt[format_index] = (*format_items_)[current_index_].Format().Inner();
        ++format_index;
    }

    if (pceltFetched) {
        *pceltFetched = format_index;
    }
    return S_OK;
}


HRESULT DataFormatEnumerator::Skip(ULONG celt) {

    auto new_index = current_index_ + celt;
    if (new_index > format_items_->size()) {
        return S_FALSE;
    }

    current_index_ = new_index;
    return S_OK;
}


HRESULT DataFormatEnumerator::Reset(void) {
    current_index_ = 0;
    return S_OK;
}


HRESULT DataFormatEnumerator::Clone(IEnumFORMATETC** ppenum) {
    return E_NOTIMPL;
}

}