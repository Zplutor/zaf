#include <zaf/control/rich_edit/internal/ole_callback_impl.h>
#include <zaf/base/as.h>
#include <zaf/clipboard/data_object.h>
#include <zaf/clipboard/drop_files_data.h>

namespace zaf::rich_edit::internal {

OLECallbackImpl::OLECallbackImpl(std::shared_ptr<OLECallback> callback) : 
    callback_(std::move(callback)) {

}


HRESULT OLECallbackImpl::QueryInterface(REFIID riid, LPVOID* ppvObj) {

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


ULONG OLECallbackImpl::AddRef() {
    return InterlockedIncrement(&reference_count_);
}


ULONG OLECallbackImpl::Release() {
    ULONG result = InterlockedDecrement(&reference_count_);
    if (result == 0) {
        delete this;
    }
    return result;
}


HRESULT OLECallbackImpl::GetNewStorage(LPSTORAGE* lplpstg) {
    return E_NOTIMPL;
}


HRESULT OLECallbackImpl::GetInPlaceContext(
    LPOLEINPLACEFRAME* lplpFrame,
    LPOLEINPLACEUIWINDOW* lplpDoc,
    LPOLEINPLACEFRAMEINFO lpFrameInfo) {
    return E_NOTIMPL;
}


HRESULT OLECallbackImpl::ShowContainerUI(BOOL fShow) {
    return E_NOTIMPL;
}


HRESULT OLECallbackImpl::QueryInsertObject(LPCLSID lpclsid, LPSTORAGE lpstg, LONG cp) {
    return S_OK;
}


HRESULT OLECallbackImpl::DeleteObject(LPOLEOBJECT lpoleobj) {
    return E_NOTIMPL;
}


HRESULT OLECallbackImpl::QueryAcceptData(
    LPDATAOBJECT lpdataobj, 
    CLIPFORMAT* lpcfFormat, 
    DWORD reco,
    BOOL fReally,
    HGLOBAL hMetaPict) {

    auto data_object_inner = COMObject<IDataObject>::FromPtr(lpdataobj);
    clipboard::DataObject data_object{ data_object_inner };

    auto expected_format_type = static_cast<clipboard::FormatType>(*lpcfFormat);

    bool accept = callback_->QueryAcceptData(
        data_object,
        expected_format_type,
        static_cast<ClipboardOperation>(reco),
        !!fReally);

    if (accept) {
        *lpcfFormat = static_cast<CLIPFORMAT>(expected_format_type);
        return S_OK;
    }
    return E_ABORT;
}


HRESULT OLECallbackImpl::ContextSensitiveHelp(BOOL fEnterMode) {
    return E_NOTIMPL;
}


HRESULT OLECallbackImpl::GetClipboardData(CHARRANGE* lpchrg, DWORD reco, LPDATAOBJECT* lplpdataobj) {

    //auto data_object = new DataObjectImpl();
    //data_object->AddFormat(std::make_shared<TextData>(L"|TextFormat|"));

    //*lplpdataobj = data_object;
    return E_NOTIMPL;
}


HRESULT OLECallbackImpl::GetDragDropEffect(BOOL fDrag, DWORD grfKeyState, LPDWORD pdwEffect) {
    return E_NOTIMPL;
}


HRESULT OLECallbackImpl::GetContextMenu(
    WORD seltype,
    LPOLEOBJECT lpoleobj,
    CHARRANGE* lpchrg,
    HMENU* lphmenu) {
    return E_NOTIMPL;
}

}