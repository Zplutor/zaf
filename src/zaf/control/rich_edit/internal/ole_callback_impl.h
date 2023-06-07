#pragma once

#include <Richedit.h>
#include <RichOle.h>
#include <zaf/control/rich_edit/ole_callback.h>

namespace zaf::rich_edit::internal {

class OLECallbackImpl : public IRichEditOleCallback {
public:
	explicit OLECallbackImpl(std::weak_ptr<OLECallback> callback);

    //IUnknown interfaces
    __declspec(nothrow) HRESULT QueryInterface(REFIID riid, LPVOID* ppvObj) override;
    __declspec(nothrow) ULONG AddRef() override;
    __declspec(nothrow) ULONG Release() override;

	//IRichEditOleCallback interfaces
	__declspec(nothrow)	HRESULT GetNewStorage(LPSTORAGE* lplpstg) override;
	__declspec(nothrow)	HRESULT GetInPlaceContext(
		LPOLEINPLACEFRAME* lplpFrame,
		LPOLEINPLACEUIWINDOW* lplpDoc,
		LPOLEINPLACEFRAMEINFO lpFrameInfo) override;
	__declspec(nothrow)	HRESULT ShowContainerUI(BOOL fShow) override;
	__declspec(nothrow)	HRESULT QueryInsertObject(
		LPCLSID lpclsid,
		LPSTORAGE lpstg, 
		LONG cp) override;
	__declspec(nothrow)	HRESULT DeleteObject(LPOLEOBJECT lpoleobj) override;
	__declspec(nothrow)	HRESULT QueryAcceptData(
		LPDATAOBJECT lpdataobj,
		CLIPFORMAT* lpcfFormat, 
		DWORD reco,
		BOOL fReally,
		HGLOBAL hMetaPict) override;
	__declspec(nothrow)	HRESULT ContextSensitiveHelp(BOOL fEnterMode) override;
	__declspec(nothrow)	HRESULT GetClipboardData(
		CHARRANGE* lpchrg, 
		DWORD reco,
		LPDATAOBJECT* lplpdataobj) override;
	__declspec(nothrow) HRESULT GetDragDropEffect(
		BOOL fDrag,
		DWORD grfKeyState,
		LPDWORD pdwEffect) override;
	__declspec(nothrow) HRESULT GetContextMenu(
		WORD seltype, 
		LPOLEOBJECT lpoleobj,
		CHARRANGE* lpchrg,
		HMENU* lphmenu) override;

private:
    LONG reference_count_{ 1 };
	std::weak_ptr<OLECallback> callback_;
};

}