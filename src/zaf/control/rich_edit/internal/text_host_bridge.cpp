#include <zaf/control/rich_edit/internal/text_host_bridge.h>
#include <zaf/control/caret.h>
#include <zaf/creation.h>
#include <zaf/graphic/alignment.h>
#include <zaf/graphic/dpi.h>
#include <zaf/rx/schedulers/main_thread_scheduler.h>
#include <zaf/rx/timer.h>
#include <zaf/window/window.h>

namespace zaf::rich_edit::internal {

TextHostBridge::TextHostBridge(const std::weak_ptr<RichEdit>& rich_edit) : rich_edit_(rich_edit) {

}


TextHostBridge::~TextHostBridge() {

}


HRESULT TextHostBridge::QueryInterface(REFIID riid, void** ppvObject) {

    if (ppvObject == nullptr) {
        return E_POINTER;
    }

    if ((riid == IID_IUnknown) || (riid == IID_ITextHost)) {
        *ppvObject = this;
        return S_OK;
    }

    *ppvObject = nullptr;
    return E_NOINTERFACE;
}


HDC TextHostBridge::TxGetDC() {
    return GetDC(GetWindowHandle());
}


INT TextHostBridge::TxReleaseDC(HDC hdc) {
    return ReleaseDC(GetWindowHandle(), hdc);
}


BOOL TextHostBridge::TxShowScrollBar(INT fnBar, BOOL fShow) {

    auto rich_edit = rich_edit_.lock();
    if (rich_edit != nullptr) {
        rich_edit->ScrollBarChange();
    }
    return TRUE;
}


BOOL TextHostBridge::TxEnableScrollBar(INT fuSBFlags, INT fuArrowflags) {

    auto rich_edit = rich_edit_.lock();
    if (rich_edit != nullptr) {
        rich_edit->ScrollBarChange();
    }
    return TRUE;
}


BOOL TextHostBridge::TxSetScrollRange(INT fnBar, LONG nMinPos, INT nMaxPos, BOOL fRedraw) {

    auto rich_edit = rich_edit_.lock();
    if (rich_edit != nullptr) {
        rich_edit->ScrollValuesChange(fnBar == SB_HORZ);
    }
    return TRUE;
}


BOOL TextHostBridge::TxSetScrollPos(INT fnBar, INT nPos, BOOL fRedraw) {

    auto rich_edit = rich_edit_.lock();
    if (rich_edit != nullptr) {
        rich_edit->ScrollValuesChange(fnBar == SB_HORZ);
    }
    return TRUE;
}


void TextHostBridge::TxInvalidateRect(LPCRECT prc, BOOL fMode) {
    auto rich_edit = rich_edit_.lock();
    if (rich_edit != nullptr) {
        rich_edit->NeedRepaint();
    }
}


void TextHostBridge::TxViewChange(BOOL fUpdate) {
}


BOOL TextHostBridge::TxCreateCaret(HBITMAP hbmp, INT xWidth, INT yHeight) {

    auto rich_edit = rich_edit_.lock();
    if (!rich_edit) {
        return FALSE;
    }

    auto window = rich_edit->Window();
    if (!window) {
        return FALSE;
    }

    float dpi = window->GetDPI();

    zaf::Size caret_size{
        ToDIPs(static_cast<float>(xWidth), dpi),
        ToDIPs(static_cast<float>(yHeight), dpi)
    };

    //Revise width. If the width is less than 1 point, it could dispear.
    if (caret_size.width < 1) {
        caret_size.width = 1;
    }

    if (!rich_edit->caret_) {
        rich_edit->caret_ = Create<Caret>(rich_edit);
    }

    rich_edit->caret_->SetSize(caret_size);
    return TRUE;
}


BOOL TextHostBridge::TxShowCaret(BOOL fShow) {

    auto rich_edit = rich_edit_.lock();
    if (!rich_edit) {
        return FALSE;
    }

    //Sometimes TxShowCaret would be called even when the 
    //rich edit is unfocused. This would mess up the caret,
    //so only response to this method while focused.
    if (!rich_edit->IsFocused()) {
        return FALSE;
    }

    if (!rich_edit->caret_) {
        return FALSE;
    }

    rich_edit->caret_->SetIsVisible(!!fShow);
    return TRUE;
}


BOOL TextHostBridge::TxSetCaretPos(INT x, INT y) {

    auto rich_edit = rich_edit_.lock();
    if (!rich_edit) {
        return FALSE;
    }

    if (!rich_edit->caret_) {
        return FALSE;
    }

    auto absolute_content_rect = rich_edit->ContentRectInWindow();
    if (!absolute_content_rect) {
        return FALSE;
    }

    //Note: x and y are in window coordinate, while the position of caret is in control content
    //coordinate.

    float dpi = rich_edit->GetDPI();
    Point caret_position{
        ToDIPs(static_cast<float>(x), dpi),
        ToDIPs(static_cast<float>(y), dpi)
    };
    
    caret_position.SubtractOffset(absolute_content_rect->position);

    rich_edit->caret_->SetPosition(caret_position);
    return TRUE;
}


BOOL TextHostBridge::TxSetTimer(UINT idTimer, UINT uTimeout) {

    auto timer = rx::Interval(
        std::chrono::milliseconds(uTimeout), 
        rx::MainThreadScheduler::Instance());

    timers_[idTimer] = timer.Subscribe([this, idTimer](std::size_t) {

        auto rich_edit = rich_edit_.lock();
        if (rich_edit) {

            auto text_service = rich_edit->GetITextServices();
            text_service->TxSendMessage(WM_TIMER, idTimer, 0, nullptr);
        }
    });

    return TRUE;
}


void TextHostBridge::TxKillTimer(UINT idTimer) {
    timers_.erase(idTimer);
}


void TextHostBridge::TxScrollWindowEx(INT dx, INT dy, LPCRECT lprcScroll, LPCRECT lprcClip, HRGN hrgnUpdate, LPRECT lprcUpdate, UINT fuScroll) {
}


void TextHostBridge::TxSetCapture(BOOL fCapture) {

    auto rich_edit = rich_edit_.lock();
    if (rich_edit == nullptr) {
        return;
    }

    if (fCapture) {
        rich_edit->CaptureMouse();
    }
    else {
        rich_edit->ReleaseMouse();
    }
}


void TextHostBridge::TxSetFocus() {

}


void TextHostBridge::TxSetCursor(HCURSOR hcur, BOOL fText) {
    SetCursor(hcur);
}


BOOL TextHostBridge::TxScreenToClient(LPPOINT lppt) {

    auto rich_edit = rich_edit_.lock();
    if (!rich_edit) {
        return FALSE;
    }

    auto window = rich_edit->Window();
    if (!window) {
        return FALSE;
    }

    return ScreenToClient(window->Handle(), lppt);
}


BOOL TextHostBridge::TxClientToScreen(LPPOINT lppt) {

    auto rich_edit = rich_edit_.lock();
    if (!rich_edit) {
        return FALSE;
    }

    auto window = rich_edit->Window();
    if (!window) {
        return FALSE;
    }

    return ClientToScreen(window->Handle(), lppt);
}


HRESULT TextHostBridge::TxActivate(LONG* plOldState) {
    return E_NOTIMPL;
}


HRESULT TextHostBridge::TxDeactivate(LONG lNewState) {
    return E_NOTIMPL;
}


//According to the documentation: "The client rectangle is expressed in client coordinates of the 
//containing window".
HRESULT TextHostBridge::TxGetClientRect(LPRECT prc) {

    *prc = RECT{};

    auto rich_edit = rich_edit_.lock();
    if (!rich_edit) {
        return S_OK;
    }

    //Returns the rect of rich edit in window.
    auto absolute_content_rect = rich_edit->ContentRectInWindow();
    if (!absolute_content_rect) {
        return S_OK;
    }

    absolute_content_rect->position.y += rich_edit->GetContentVerticalOffset();

    auto aligned_rect = ToPixelAlignedInPixels(*absolute_content_rect, rich_edit->GetDPI());
    *prc = aligned_rect.ToRECT();
    return S_OK;
}


HRESULT TextHostBridge::TxGetViewInset(LPRECT prc) {

    *prc = RECT{};

    auto rich_edit = rich_edit_.lock();
    if (!rich_edit) {
        return S_OK;
    }

    auto window = rich_edit->Window();
    if (!window) {
        return S_OK;
    }

    *prc = FromDIPs(rich_edit->GetInset(), window->GetDPI()).ToRECT();
    return S_OK;
}


HRESULT TextHostBridge::TxGetCharFormat(const CHARFORMATW** ppCF) {

    auto rich_edit = rich_edit_.lock();
    if (rich_edit != nullptr) {
        *ppCF = &(rich_edit->character_format_);
        return S_OK;
    }
    else {
        return E_NOTIMPL;
    }
}


HRESULT TextHostBridge::TxGetParaFormat(const PARAFORMAT** ppPF) {

    auto rich_edit = rich_edit_.lock();
    if (rich_edit != nullptr) {
        *ppPF = &(rich_edit->paragraph_format_);
        return S_OK;
    }
    else {
        return E_NOTIMPL;
    }
}


COLORREF TextHostBridge::TxGetSysColor(int nIndex) {

    auto rich_edit = rich_edit_.lock();
    if (!rich_edit) {
        return 0;
    }

    //Use custom colors for selected text if rich edit doesn't have focus.
    if (!rich_edit->ContainsFocus()) {

        if (nIndex == COLOR_HIGHLIGHT) {
            return RGB(0xE4, 0xE4, 0xE4);
        }

        if (nIndex == COLOR_HIGHLIGHTTEXT) {
            return RGB(0, 0, 0);
        }
    }

    return GetSysColor(nIndex);
}


HRESULT TextHostBridge::TxGetBackStyle(TXTBACKSTYLE* pstyle) {
    *pstyle = TXTBACK_TRANSPARENT;
    return S_OK;
}


HRESULT TextHostBridge::TxGetMaxLength(DWORD* plength) {

    auto rich_edit = rich_edit_.lock();
    if (rich_edit) {
        *plength = rich_edit->GetMaxLength();
    }
    return S_OK;
}


HRESULT TextHostBridge::TxGetScrollBars(DWORD* pdwScrollBar) {

    auto rich_edit = rich_edit_.lock();
    if (rich_edit) {
        *pdwScrollBar = rich_edit->scroll_bar_property_;
    }
    return S_OK;
}


HRESULT TextHostBridge::TxGetPasswordChar(_Out_ TCHAR* pch) {

    auto rich_edit = rich_edit_.lock();
    if (rich_edit) {
        *pch = rich_edit->GetPasswordCharacter();
    }
    return S_OK;
}


HRESULT TextHostBridge::TxGetAcceleratorPos(LONG* pcp) {
    return E_NOTIMPL;
}


HRESULT TextHostBridge::TxGetExtent(LPSIZEL lpExtent) {
    return E_NOTIMPL;
}


HRESULT TextHostBridge::OnTxCharFormatChange(const CHARFORMATW* pCF) {
    return E_NOTIMPL;
}


HRESULT TextHostBridge::OnTxParaFormatChange(const PARAFORMAT* pPF) {
    return E_NOTIMPL;
}


HRESULT TextHostBridge::TxGetPropertyBits(DWORD dwMask, DWORD* pdwBits) {

    auto rich_edit = rich_edit_.lock();
    if (rich_edit) {
        *pdwBits = dwMask & rich_edit->property_bits_;
    }
    return S_OK;
}


HRESULT TextHostBridge::TxNotify(DWORD iNotify, void* pv) {

    auto rich_edit = rich_edit_.lock();
    if (rich_edit == nullptr) {
        return S_OK;
    }

    switch (iNotify) {
    case EN_CHANGE: {
        //Reset cached text height to update vertical alignment position.
        rich_edit->ResetCachedTextHeight();
        rich_edit->HandleTextChangedNotification();
        return S_OK;
    }

    case EN_SELCHANGE: {
        rich_edit->HandleSelectionChangedNotification();
        return S_OK;
    }

    case EN_PROTECTED: {
        bool allowed = NotifyProtected(*reinterpret_cast<ENPROTECTED*>(pv));
        return allowed ? S_OK : S_FALSE;
    }

    default:
        return S_OK;
    }
}


bool TextHostBridge::NotifyProtected(const ENPROTECTED& info) const {

    auto rich_edit = rich_edit_.lock();
    if (!rich_edit) {
        return false;
    }

    bool can_change = rich_edit->HandleProtectedNotification(info);
    if (!can_change) {
        return false;
    }

    auto validator = rich_edit->GetTextValidator();
    if (!validator) {
        return true;
    }

    auto get_new_text = [&info](std::wstring& new_text) -> bool {

        if (info.msg == WM_CHAR) {
            new_text.assign(1, static_cast<wchar_t>(info.wParam));
            return true;
        }

        if (info.msg == WM_PASTE) {

            BOOL is_succeeded = OpenClipboard(nullptr);
            if (!is_succeeded) {
                return false;
            }

            bool result = false;
            HANDLE data = GetClipboardData(CF_UNICODETEXT);
            if (data != nullptr) {
                new_text.assign(reinterpret_cast<wchar_t*>(data));
                result = true;
            }

            CloseClipboard();
            return result;
        }

        return false;
    };

    std::wstring new_text;
    bool has_got_new_text = get_new_text(new_text);
    if (has_got_new_text) {
        return validator(*rich_edit, new_text);
    }
    else {
        return true;
    }
}


HIMC TextHostBridge::TxImmGetContext() {

    auto window = GetWindow();
    if (window) {
        return ImmGetContext(window->Handle());
    }
    return nullptr;
}


void TextHostBridge::TxImmReleaseContext(HIMC himc) {

    auto window = GetWindow();
    if (window) {
        ImmReleaseContext(window->Handle(), himc);
    }
}


HRESULT TextHostBridge::TxGetSelectionBarWidth(LONG* lSelBarWidth) {
    *lSelBarWidth = 0;
    return S_OK;
}


std::shared_ptr<Window> TextHostBridge::GetWindow() const {

    auto rich_edit = rich_edit_.lock();
    if (rich_edit == nullptr) {
        return nullptr;
    }

    return rich_edit->Window();
}


HWND TextHostBridge::GetWindowHandle() const {

    auto window = GetWindow();
    if (window == nullptr) {
        return nullptr;
    }

    return window->Handle();
}

}