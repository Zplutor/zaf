#pragma once

#include <oleidl.h>
#include <zaf/control/rich_edit/embedded_object.h>

namespace zaf::rich_edit::internal {

class OLEObjectImpl : public IOleObject {
public:
    explicit OLEObjectImpl(std::shared_ptr<EmbeddedObject> embedded_object);

    const std::shared_ptr<EmbeddedObject>& EmbeddedObject() const {
        return embedded_object_;
    }

    //IUnknown interfaces
    HRESULT QueryInterface(REFIID riid, LPVOID* ppvObj) override;
    ULONG AddRef() override;
    ULONG Release() override;

    //IOleObject interfaces
    HRESULT SetClientSite(IOleClientSite* pClientSite) override;
    HRESULT GetClientSite(IOleClientSite** ppClientSite) override;
    HRESULT SetHostNames(LPCOLESTR szContainerApp, LPCOLESTR szContainerObj) override;
    HRESULT Close(DWORD dwSaveOption) override;
    HRESULT SetMoniker(DWORD dwWhichMoniker, IMoniker* pmk) override;
    HRESULT GetMoniker(DWORD dwAssign, DWORD dwWhichMoniker, IMoniker** ppmk) override;
    HRESULT InitFromData(IDataObject* pDataObject, BOOL fCreation, DWORD dwReserved) override;
    HRESULT GetClipboardData(DWORD dwReserved, IDataObject** ppDataObject) override;
    HRESULT DoVerb(
        LONG iVerb,
        LPMSG lpmsg,
        IOleClientSite* pActiveSite,
        LONG lindex,
        HWND hwndParent,
        LPCRECT lprcPosRect) override;
    HRESULT EnumVerbs(IEnumOLEVERB** ppEnumOleVerb) override;
    HRESULT Update(void) override;
    HRESULT IsUpToDate(void) override;
    HRESULT GetUserClassID(CLSID* pClsid) override;
    HRESULT GetUserType(DWORD dwFormOfType, LPOLESTR* pszUserType) override;
    HRESULT SetExtent(DWORD dwDrawAspect, SIZEL* psizel) override;
    HRESULT GetExtent(DWORD dwDrawAspect, SIZEL* psizel) override;
    HRESULT Advise(IAdviseSink* pAdvSink, DWORD* pdwConnection) override;
    HRESULT Unadvise(DWORD dwConnection) override;
    HRESULT EnumAdvise(IEnumSTATDATA** ppenumAdvise) override;
    HRESULT GetMiscStatus(DWORD dwAspect, DWORD* pdwStatus) override;
    HRESULT SetColorScheme(LOGPALETTE* pLogpal) override;

private:
    LONG reference_count_{ 1 };
    std::shared_ptr<rich_edit::EmbeddedObject> embedded_object_;
};

}