#include <zaf/control/rich_edit/embedded_object.h>

namespace zaf::rich_edit {

bool EmbeddedObject::ChangeMouseCursor(bool is_in_selection_range) {
    return false;
}


void EmbeddedObject::OnMouseDown(const MouseDownContext& context) {

}


void EmbeddedObject::OnMouseUp(const MouseUpContext& context) {

}


void EmbeddedObject::OnDoubleClick(const DoubleClickContext& context) {

}


HRESULT EmbeddedObject::QueryInterface(REFIID riid, LPVOID* ppvObj) {

    if (!ppvObj) {
        return E_INVALIDARG;
    }

    if (riid == IID_IUnknown || riid == IID_IOleObject) {
        *ppvObj = this;
        AddRef();
        return S_OK;
    }

    *ppvObj = nullptr;
    return E_NOINTERFACE;
}


ULONG EmbeddedObject::AddRef() {
    return InterlockedIncrement(&reference_count_);
}


ULONG EmbeddedObject::Release() {
    ULONG result = InterlockedDecrement(&reference_count_);
    if (result == 0) {
        delete this;
    }
    return result;
}


HRESULT EmbeddedObject::SetClientSite(IOleClientSite* pClientSite) {
    return E_NOTIMPL;
}


HRESULT EmbeddedObject::GetClientSite(IOleClientSite** ppClientSite) {
    return E_NOTIMPL;
}


HRESULT EmbeddedObject::SetHostNames(LPCOLESTR szContainerApp, LPCOLESTR szContainerObj) {
    return E_NOTIMPL;
}


HRESULT EmbeddedObject::Close(DWORD dwSaveOption) {
    return E_NOTIMPL;
}


HRESULT EmbeddedObject::SetMoniker(DWORD dwWhichMoniker, IMoniker* pmk) {
    return E_NOTIMPL;
}


HRESULT EmbeddedObject::GetMoniker(DWORD dwAssign, DWORD dwWhichMoniker, IMoniker** ppmk) {
    return E_NOTIMPL;
}


HRESULT EmbeddedObject::InitFromData(IDataObject* pDataObject, BOOL fCreation, DWORD dwReserved) {
    return E_NOTIMPL;
}


HRESULT EmbeddedObject::GetClipboardData(DWORD dwReserved, IDataObject** ppDataObject) {
    return E_NOTIMPL;
}


HRESULT EmbeddedObject::DoVerb(
    LONG iVerb,
    LPMSG lpmsg,
    IOleClientSite* pActiveSite,
    LONG lindex,
    HWND hwndParent,
    LPCRECT lprcPosRect) {

    return E_NOTIMPL;
}


HRESULT EmbeddedObject::EnumVerbs(IEnumOLEVERB** ppEnumOleVerb) {
    return E_NOTIMPL;
}


HRESULT EmbeddedObject::Update(void) {
    return E_NOTIMPL;
}


HRESULT EmbeddedObject::IsUpToDate(void) {
    return E_NOTIMPL;
}


HRESULT EmbeddedObject::GetUserClassID(CLSID* pClsid) {
    *pClsid = this->ClassID();
    return S_OK;
}


HRESULT EmbeddedObject::GetUserType(DWORD dwFormOfType, LPOLESTR* pszUserType) {
    return E_NOTIMPL;
}


HRESULT EmbeddedObject::SetExtent(DWORD dwDrawAspect, SIZEL* psizel) {
    return E_NOTIMPL;
}


HRESULT EmbeddedObject::GetExtent(DWORD dwDrawAspect, SIZEL* psizel) {
    return E_NOTIMPL;
}


HRESULT EmbeddedObject::Advise(IAdviseSink* pAdvSink, DWORD* pdwConnection) {
    return S_OK;
}


HRESULT EmbeddedObject::Unadvise(DWORD dwConnection) {
    return E_NOTIMPL;
}


HRESULT EmbeddedObject::EnumAdvise(IEnumSTATDATA** ppenumAdvise) {
    return E_NOTIMPL;
}


HRESULT EmbeddedObject::GetMiscStatus(DWORD dwAspect, DWORD* pdwStatus) {
    return E_NOTIMPL;
}


HRESULT EmbeddedObject::SetColorScheme(LOGPALETTE* pLogpal) {
    return E_NOTIMPL;
}

}