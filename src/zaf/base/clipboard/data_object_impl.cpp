#include <zaf/base/clipboard/data_object_impl.h>
#include <zaf/base/clipboard/data_format_enumerator.h>

namespace zaf {
namespace {

bool AreFORMATETCsEqual(const FORMATETC& format1, const FORMATETC& format2) {
    return
        (format1.cfFormat == format2.cfFormat) &&
        (format1.dwAspect == format2.dwAspect) &&
        (format1.lindex == format2.lindex) &&
        (format1.ptd == format2.ptd) &&
        (format1.tymed == format2.tymed);
}

}

DataObjectImpl::DataObjectImpl() : 
    format_items_(std::make_shared<internal::DataFormatItemList>()) {

}


void DataObjectImpl::SetFormatData(const DataFormat& format, std::shared_ptr<ClipboardData> data) {
    format_items_->emplace_back(format, std::move(data));
}


HRESULT DataObjectImpl::QueryInterface(REFIID riid, LPVOID* ppvObj) {

    if (!ppvObj) {
        return E_INVALIDARG;
    }

    if (riid == IID_IUnknown || riid == IID_IDataObject) {
        *ppvObj = this;
        AddRef();
        return S_OK;
    }

    *ppvObj = nullptr;
    return E_NOINTERFACE;
}


ULONG DataObjectImpl::AddRef() {
    return InterlockedIncrement(&reference_count_);
}


ULONG DataObjectImpl::Release() {
    ULONG result = InterlockedDecrement(&reference_count_);
    if (result == 0) {
        delete this;
    }
    return result;
}


HRESULT DataObjectImpl::GetData(FORMATETC* pformatetcIn, STGMEDIUM* pmedium) {

    for (const auto& each_item : *format_items_) {

        if (AreFORMATETCsEqual(each_item.Format().Inner(), *pformatetcIn)) {

            auto medium = each_item.Data()->SaveToMedium(each_item.Format());
            *pmedium = medium.Detach();
            return S_OK;
        }
    }

    return DV_E_FORMATETC;;
}


HRESULT DataObjectImpl::GetDataHere(FORMATETC* pformatetc, STGMEDIUM* pmedium) {
    return E_NOTIMPL;
}


HRESULT DataObjectImpl::QueryGetData(FORMATETC* pformatetc) {
    return E_NOTIMPL;
}


HRESULT DataObjectImpl::GetCanonicalFormatEtc(FORMATETC* pformatectIn, FORMATETC* pformatetcOut) {
    return E_NOTIMPL;
}


HRESULT DataObjectImpl::SetData(FORMATETC* pformatetc, STGMEDIUM* pmedium, BOOL fRelease) {
    return E_NOTIMPL;
}


HRESULT DataObjectImpl::EnumFormatEtc(DWORD dwDirection, IEnumFORMATETC** ppenumFormatEtc) {

    if (dwDirection == DATADIR_SET) {
        return E_NOTIMPL;
    }

    *ppenumFormatEtc = new DataFormatEnumerator{ format_items_ };
    return S_OK;
}


HRESULT DataObjectImpl::DAdvise(
    FORMATETC* pformatetc, 
    DWORD advf,
    IAdviseSink* pAdvSink,
    DWORD* pdwConnection) {
    return E_NOTIMPL;
}


HRESULT DataObjectImpl::DUnadvise(DWORD dwConnection) {
    return E_NOTIMPL;
}


HRESULT DataObjectImpl::EnumDAdvise(IEnumSTATDATA** ppenumAdvise) {
    return E_NOTIMPL;
}

}