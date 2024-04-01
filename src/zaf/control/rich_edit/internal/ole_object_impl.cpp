#include <zaf/control/rich_edit/internal/ole_object_impl.h>
#include <zaf/base/error/contract.h>

namespace zaf::rich_edit::internal {

OLEObjectImpl::OLEObjectImpl(std::shared_ptr<rich_edit::EmbeddedObject> embedded_object) : 
    embedded_object_(embedded_object) {

    ZAF_EXPECT(embedded_object_);
}


HRESULT OLEObjectImpl::QueryInterface(REFIID riid, LPVOID* ppvObj) {

    if (!ppvObj) {
        return E_INVALIDARG;
    }

    if (riid == IID_IUnknown || riid == IID_IOleObject || riid == IID_OLEObjectImpl) {
        *ppvObj = this;
        AddRef();
        return S_OK;
    }

    *ppvObj = nullptr;
    return E_NOINTERFACE;
}


ULONG OLEObjectImpl::AddRef() {
    return InterlockedIncrement(&reference_count_);
}


ULONG OLEObjectImpl::Release() {
    ULONG result = InterlockedDecrement(&reference_count_);
    if (result == 0) {
        delete this;
    }
    return result;
}


HRESULT OLEObjectImpl::SetClientSite(IOleClientSite* pClientSite) {
    return E_NOTIMPL;
}


HRESULT OLEObjectImpl::GetClientSite(IOleClientSite** ppClientSite) {
    return E_NOTIMPL;
}


HRESULT OLEObjectImpl::SetHostNames(LPCOLESTR szContainerApp, LPCOLESTR szContainerObj) {
    return E_NOTIMPL;
}


HRESULT OLEObjectImpl::Close(DWORD dwSaveOption) {
    return E_NOTIMPL;
}


HRESULT OLEObjectImpl::SetMoniker(DWORD dwWhichMoniker, IMoniker* pmk) {
    return E_NOTIMPL;
}


HRESULT OLEObjectImpl::GetMoniker(DWORD dwAssign, DWORD dwWhichMoniker, IMoniker** ppmk) {
    return E_NOTIMPL;
}


HRESULT OLEObjectImpl::InitFromData(IDataObject* pDataObject, BOOL fCreation, DWORD dwReserved) {
    return E_NOTIMPL;
}


HRESULT OLEObjectImpl::GetClipboardData(DWORD dwReserved, IDataObject** ppDataObject) {
    return E_NOTIMPL;
}


HRESULT OLEObjectImpl::DoVerb(
    LONG iVerb,
    LPMSG lpmsg,
    IOleClientSite* pActiveSite,
    LONG lindex,
    HWND hwndParent,
    LPCRECT lprcPosRect) {

    return E_NOTIMPL;
}


HRESULT OLEObjectImpl::EnumVerbs(IEnumOLEVERB** ppEnumOleVerb) {
    return E_NOTIMPL;
}


HRESULT OLEObjectImpl::Update(void) {
    return E_NOTIMPL;
}


HRESULT OLEObjectImpl::IsUpToDate(void) {
    return E_NOTIMPL;
}


HRESULT OLEObjectImpl::GetUserClassID(CLSID* pClsid) {
    *pClsid = embedded_object_->ClassID();
    return S_OK;
}


HRESULT OLEObjectImpl::GetUserType(DWORD dwFormOfType, LPOLESTR* pszUserType) {
    return E_NOTIMPL;
}


HRESULT OLEObjectImpl::SetExtent(DWORD dwDrawAspect, SIZEL* psizel) {
    return E_NOTIMPL;
}


HRESULT OLEObjectImpl::GetExtent(DWORD dwDrawAspect, SIZEL* psizel) {
    return E_NOTIMPL;
}


HRESULT OLEObjectImpl::Advise(IAdviseSink* pAdvSink, DWORD* pdwConnection) {
    return S_OK;
}


HRESULT OLEObjectImpl::Unadvise(DWORD dwConnection) {
    return E_NOTIMPL;
}


HRESULT OLEObjectImpl::EnumAdvise(IEnumSTATDATA** ppenumAdvise) {
    return E_NOTIMPL;
}


HRESULT OLEObjectImpl::GetMiscStatus(DWORD dwAspect, DWORD* pdwStatus) {
    return E_NOTIMPL;
}


HRESULT OLEObjectImpl::SetColorScheme(LOGPALETTE* pLogpal) {
    return E_NOTIMPL;
}

}