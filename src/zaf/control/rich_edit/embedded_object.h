#pragma once

#include <oleidl.h>
#include <zaf/base/non_copyable.h>
#include <zaf/graphic/rect.h>
#include <zaf/graphic/size.h>

namespace zaf {

class Canvas;

namespace rich_edit {

class EmbeddedObject : public IOleObject, NonCopyable {
public:
    const zaf::Size& Size() const {
        return size_;
    }

    void SetSize(const zaf::Size& size) {
        size_ = size;
    }

    virtual GUID ClassID() const = 0;

    /**
    Paints the embedded object. 

    @param canvas
        The canvas to which the object paints.

    @param dirty_rect
        A rectangle area that needs to be painted, in object coordindate.
    */
    virtual void Paint(Canvas& canvas, const zaf::Rect& dirty_rect) = 0;

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
    zaf::Size size_;
};

}
}