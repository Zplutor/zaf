#include <zaf/control/rich_edit/internal/ole_callback_impl.h>
#include <zaf/base/as.h>
#include <zaf/clipboard/data_object.h>
#include <zaf/clipboard/drop_files_data.h>

namespace zaf::rich_edit::internal {

OLECallbackImpl::OLECallbackImpl(std::weak_ptr<OLECallback> callback) :
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

    auto callback = callback_.lock();
    if (!callback) {
        assert(false);
        return E_NOTIMPL;
    }

    clipboard::DataObject data_object{ ToCOMPtrNotOwn(lpdataobj) };

    auto expected_format_type = static_cast<clipboard::FormatType>(*lpcfFormat);

    OperationResult operation_result{ OperationResult::Pending };
    if (fReally) {
        operation_result = callback->InsertClipboardData(
            static_cast<ClipboardOperation>(reco),
            data_object,
            expected_format_type);
    }
    else {
        operation_result = callback->CanInsertClipboardData(
            static_cast<ClipboardOperation>(reco), 
            data_object,
            expected_format_type);
    }

    switch (operation_result) {
    case OperationResult::OK:
        return S_FALSE;
    case OperationResult::Pending:
        *lpcfFormat = static_cast<CLIPFORMAT>(expected_format_type);
        return S_OK;
    default:
        return E_ABORT;
    }
}


HRESULT OLECallbackImpl::ContextSensitiveHelp(BOOL fEnterMode) {
    return E_NOTIMPL;
}


HRESULT OLECallbackImpl::GetClipboardData(
    CHARRANGE* lpchrg,
    DWORD reco, 
    LPDATAOBJECT* lplpdataobj) {

    auto callback = callback_.lock();
    if (!callback) {
        assert(false);
        return E_NOTIMPL;
    }

    auto data_object = clipboard::DataObject::Create();
    auto operation_result = callback->GetClipboardData(
        static_cast<ClipboardOperation>(reco),
        Range::FromCHARRANGE(*lpchrg),
        data_object);

    switch (operation_result) {
    case OperationResult::OK: {
        auto data_object_ptr = data_object.Inner();
        *lplpdataobj = data_object_ptr.Detach();
        return S_OK;
    }
    case OperationResult::Pending:
        return E_NOTIMPL;
    default:
        return E_ABORT;
    }
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