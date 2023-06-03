#include <zaf/base/clipboard/data_object.h>
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

DataObject::DataObject() : formats_(std::make_shared<DataFormatList>()) {

}


void DataObject::AddFormat(std::shared_ptr<DataFormat> format) {
    formats_->push_back(std::move(format));
}


HRESULT DataObject::QueryInterface(REFIID riid, LPVOID* ppvObj) {

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


ULONG DataObject::AddRef() {
    return InterlockedIncrement(&reference_count_);
}


ULONG DataObject::Release() {
    ULONG result = InterlockedDecrement(&reference_count_);
    if (result == 0) {
        delete this;
    }
    return result;
}


HRESULT DataObject::GetData(FORMATETC* pformatetcIn, STGMEDIUM* pmedium) {

    for (const auto& each_format : *formats_) {

        auto format_info = each_format->GetFORMATECT();
        if (AreFORMATETCsEqual(*pformatetcIn, format_info)) {
            *pmedium = each_format->GetSTGMEDIUM();
            return S_OK;
        }
    }

    return DV_E_FORMATETC;;
}


HRESULT DataObject::GetDataHere(FORMATETC* pformatetc, STGMEDIUM* pmedium) {
    return E_NOTIMPL;
}


HRESULT DataObject::QueryGetData(FORMATETC* pformatetc) {
    return E_NOTIMPL;
}


HRESULT DataObject::GetCanonicalFormatEtc(FORMATETC* pformatectIn, FORMATETC* pformatetcOut) {
    return E_NOTIMPL;
}


HRESULT DataObject::SetData(FORMATETC* pformatetc, STGMEDIUM* pmedium, BOOL fRelease) {
    return E_NOTIMPL;
}


HRESULT DataObject::EnumFormatEtc(DWORD dwDirection, IEnumFORMATETC** ppenumFormatEtc) {

    if (dwDirection == DATADIR_SET) {
        return E_NOTIMPL;
    }

    *ppenumFormatEtc = new DataFormatEnumerator(formats_);
    return S_OK;
}


HRESULT DataObject::DAdvise(
    FORMATETC* pformatetc, 
    DWORD advf,
    IAdviseSink* pAdvSink,
    DWORD* pdwConnection) {
    return E_NOTIMPL;
}


HRESULT DataObject::DUnadvise(DWORD dwConnection) {
    return E_NOTIMPL;
}


HRESULT DataObject::EnumDAdvise(IEnumSTATDATA** ppenumAdvise) {
    return E_NOTIMPL;
}

}