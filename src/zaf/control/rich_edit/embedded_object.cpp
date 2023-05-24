#include <zaf/control/rich_edit/embedded_object.h>
#include <TOM.h>
#include <zaf/base/container/utility/range.h>
#include <zaf/control/rich_edit.h>
#include <zaf/graphic/dpi.h>

namespace zaf::rich_edit {
namespace {

std::optional<std::size_t> GetObjectCharIndex(
    const OLEInterface& ole_interface,
    const EmbeddedObject* object) {

    //Loop over all objects to find index of current object.
    for (auto index : zaf::Range(0, ole_interface.GetObjectCount())) {

        auto object_info = ole_interface.GetObjectInfoAt(index);
        if (object == object_info.Object().As<EmbeddedObject>().Inner()) {
            return object_info.Index();
        }
    }

    return std::nullopt;
}


Point GetObjectPositionInScreenInPixels(
    const OLEInterface& ole_interface, 
    std::size_t char_index) {

    auto text_document = ole_interface.Query<ITextDocument>();
    if (!text_document) {
        return {};
    }

    COMObject<ITextRange> text_range;
    HRESULT hresult = text_document->Range(
        static_cast<long>(char_index), 
        static_cast<long>(char_index + 1), 
        text_range.Reset());

    ZAF_THROW_IF_COM_ERROR(hresult);

    long object_x{};
    long object_y{};
    hresult = text_range->GetPoint(tomStart | TA_TOP | TA_LEFT, &object_x, &object_y);
    ZAF_THROW_IF_COM_ERROR(hresult);

    return Point{ static_cast<float>(object_x), static_cast<float>(object_y) };
}

}

Point EmbeddedObject::GetPositionInScreen() const {

    auto host = Host();
    if (!host) {
        return {};
    }

    try {

        auto ole_interface = host->GetOLEInterface();
        auto object_index = GetObjectCharIndex(ole_interface, this);
        if (!object_index) {
            return {};
        }

        auto position_in_pixels = GetObjectPositionInScreenInPixels(ole_interface, *object_index);
        return ToDIPs(position_in_pixels, host->GetDPI());
    }
    catch (const Error&) {
        return {};
    }
}


void EmbeddedObject::OnMouseCursorChanging(const MouseCursorChangingContext& context) {

}


void EmbeddedObject::OnMouseDown(const MouseDownContext& context) {

}


void EmbeddedObject::OnMouseUp(const MouseUpContext& context) {

}


bool EmbeddedObject::OnDoubleClick(const DoubleClickContext& context) {
    return false;
}


void EmbeddedObject::NeedRepaint() {

    auto host = Host();
    if (host) {
        host->NeedRepaint();
    }
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