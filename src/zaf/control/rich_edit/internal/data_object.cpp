#include <zaf/control/rich_edit/internal/data_object.h>
#include <zaf/control/rich_edit/internal/data_format_enumerator.h>

namespace zaf::rich_edit::internal {

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

    std::wstring string = L"|str|";

    std::size_t memory_size = string.size() * sizeof(wchar_t*);
    auto hglobal = GlobalAlloc(GMEM_MOVEABLE, memory_size);
    auto memory = GlobalLock(hglobal);
    std::memcpy(memory, string.data(), memory_size);
    GlobalUnlock(hglobal);

    pmedium->tymed = TYMED_HGLOBAL;
    pmedium->hGlobal = hglobal;

    return S_OK;
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

    *ppenumFormatEtc = new DataFormatEnumerator();
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