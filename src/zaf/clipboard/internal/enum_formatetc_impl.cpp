#include <zaf/clipboard/internal/enum_formatetc_impl.h>

namespace zaf::clipboard::internal {

EnumFORMATETCImpl::EnumFORMATETCImpl(std::shared_ptr<FormatItemList> format_items) :
    format_items_(std::move(format_items)) {

}


HRESULT EnumFORMATETCImpl::QueryInterface(REFIID riid, LPVOID* ppvObj) {

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


ULONG EnumFORMATETCImpl::AddRef() {
    return InterlockedIncrement(&reference_count_);
}


ULONG EnumFORMATETCImpl::Release() {
    ULONG result = InterlockedDecrement(&reference_count_);
    if (result == 0) {
        delete this;
    }
    return result;
}


HRESULT EnumFORMATETCImpl::Next(ULONG celt, FORMATETC* rgelt, ULONG* pceltFetched) {

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


HRESULT EnumFORMATETCImpl::Skip(ULONG celt) {

    auto new_index = current_index_ + celt;
    if (new_index > format_items_->size()) {
        return S_FALSE;
    }

    current_index_ = new_index;
    return S_OK;
}


HRESULT EnumFORMATETCImpl::Reset(void) {
    current_index_ = 0;
    return S_OK;
}


HRESULT EnumFORMATETCImpl::Clone(IEnumFORMATETC** ppenum) {
    return E_NOTIMPL;
}

}