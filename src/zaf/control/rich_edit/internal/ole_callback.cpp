#include <zaf/control/rich_edit/internal/ole_callback.h>
#include <zaf/clipboard/data_object.h>

namespace zaf::rich_edit::internal {

OLECallback::OLECallback(std::weak_ptr<RichEdit> host) : host_(std::move(host)) {

}


HRESULT OLECallback::QueryInterface(REFIID riid, LPVOID* ppvObj) {

    if (!ppvObj) {
        return E_INVALIDARG;
    }

    if (riid == IID_IUnknown || riid == IID_IRichEditOleCallback) {
        *ppvObj = this;
        AddRef();
        return S_OK;
    }

    *ppvObj = nullptr;
    return E_NOINTERFACE;
}


ULONG OLECallback::AddRef() {
    return InterlockedIncrement(&reference_count_);
}


ULONG OLECallback::Release() {
    ULONG result = InterlockedDecrement(&reference_count_);
    if (result == 0) {
        delete this;
    }
    return result;
}


HRESULT OLECallback::GetNewStorage(LPSTORAGE* lplpstg) {
    return E_NOTIMPL;
}


HRESULT OLECallback::GetInPlaceContext(
    LPOLEINPLACEFRAME* lplpFrame,
    LPOLEINPLACEUIWINDOW* lplpDoc,
    LPOLEINPLACEFRAMEINFO lpFrameInfo) {
    return E_NOTIMPL;
}


HRESULT OLECallback::ShowContainerUI(BOOL fShow) {
    return E_NOTIMPL;
}


HRESULT OLECallback::QueryInsertObject(LPCLSID lpclsid, LPSTORAGE lpstg, LONG cp) {
    return S_OK;
}


HRESULT OLECallback::DeleteObject(LPOLEOBJECT lpoleobj) {
    return E_NOTIMPL;
}


HRESULT OLECallback::QueryAcceptData(
    LPDATAOBJECT lpdataobj, 
    CLIPFORMAT* lpcfFormat, 
    DWORD reco,
    BOOL fReally,
    HGLOBAL hMetaPict) {

    auto data_object_inner = COMObject<IDataObject>::FromPtr(lpdataobj);
    clipboard::DataObject data_object{ data_object_inner };

    auto enumerator = data_object.EnumerateFormats();
    while (auto format = enumerator.Next()) {

        auto t = format->Type();
        auto mt = format->MediumType();
    }

    auto t = data_object.GetText();

    return E_NOTIMPL;
}


HRESULT OLECallback::ContextSensitiveHelp(BOOL fEnterMode) {
    return E_NOTIMPL;
}


HRESULT OLECallback::GetClipboardData(CHARRANGE* lpchrg, DWORD reco, LPDATAOBJECT* lplpdataobj) {

    //auto data_object = new DataObjectImpl();
    //data_object->AddFormat(std::make_shared<TextData>(L"|TextFormat|"));

    //*lplpdataobj = data_object;
    return E_NOTIMPL;
}


HRESULT OLECallback::GetDragDropEffect(BOOL fDrag, DWORD grfKeyState, LPDWORD pdwEffect) {
    return E_NOTIMPL;
}


HRESULT OLECallback::GetContextMenu(
    WORD seltype,
    LPOLEOBJECT lpoleobj,
    CHARRANGE* lpchrg,
    HMENU* lphmenu) {
    return E_NOTIMPL;
}

}