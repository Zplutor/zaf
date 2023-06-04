#include <zaf/clipboard/internal/data_object_impl.h>
#include <zaf/clipboard/internal/format_enumerator.h>
#include <zaf/clipboard/internal/medium_wrapping.h>

namespace zaf::clipboard::internal {
namespace {

Medium DeepCopyMedium(const FORMATETC& format, const STGMEDIUM& medium) {

    auto new_medium = ShallowCopySTGMEDIUM(medium);
    switch (new_medium.tymed) {
        case TYMED_HGLOBAL:
            new_medium.hGlobal = OleDuplicateData(new_medium.hGlobal, format.cfFormat, 0);
            break;
        case TYMED_GDI:
            new_medium.hBitmap = reinterpret_cast<HBITMAP>(OleDuplicateData(
                new_medium.hBitmap,
                format.cfFormat, 
                0));
            break;
        case TYMED_MFPICT:
            new_medium.hMetaFilePict = OleDuplicateData(
                new_medium.hMetaFilePict, 
                format.cfFormat,
                0);
            break;
        case TYMED_ENHMF:
            new_medium.hEnhMetaFile = reinterpret_cast<HENHMETAFILE>(OleDuplicateData(
                new_medium.hEnhMetaFile, 
                format.cfFormat,
                0));
            break;
        default:
            break;
    }
    return Medium{ new_medium };
}

}

DataObjectImpl::DataObjectImpl() : format_items_(std::make_shared<FormatItemList>()) {

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

        if (each_item.Format() == *pformatetcIn) {

            auto medium = each_item.Medium();
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

    Format format{ *pformatetc };
    if (fRelease) {
        format_items_->emplace_back(format, Medium{ *pmedium });
    }
    else {
        format_items_->emplace_back(format, DeepCopyMedium(*pformatetc, *pmedium));
    }
    return S_OK;
}


HRESULT DataObjectImpl::EnumFormatEtc(DWORD dwDirection, IEnumFORMATETC** ppenumFormatEtc) {

    if (dwDirection == DATADIR_SET) {
        return E_NOTIMPL;
    }

    *ppenumFormatEtc = new FormatEnumerator{ format_items_ };
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