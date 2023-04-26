#include <zaf/control/rich_edit/internal/text_host_bridge.h>
#include <zaf/control/caret.h>
#include <zaf/creation.h>
#include <zaf/graphic/alignment.h>
#include <zaf/graphic/dpi.h>
#include <zaf/rx/scheduler.h>
#include <zaf/rx/timer.h>
#include <zaf/window/window.h>

namespace zaf::rich_edit::internal {

TextHostBridge::TextHostBridge(const std::weak_ptr<TextBox>& text_box) : text_box_(text_box) {

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

    auto text_box = text_box_.lock();
    if (text_box != nullptr) {
        text_box->ScrollBarChange();
    }
    return TRUE;
}


BOOL TextHostBridge::TxEnableScrollBar(INT fuSBFlags, INT fuArrowflags) {

    auto text_box = text_box_.lock();
    if (text_box != nullptr) {
        text_box->ScrollBarChange();
    }
    return TRUE;
}


BOOL TextHostBridge::TxSetScrollRange(INT fnBar, LONG nMinPos, INT nMaxPos, BOOL fRedraw) {

    auto text_box = text_box_.lock();
    if (text_box != nullptr) {
        text_box->ScrollValuesChange(fnBar == SB_HORZ);
    }
    return TRUE;
}


BOOL TextHostBridge::TxSetScrollPos(INT fnBar, INT nPos, BOOL fRedraw) {

    auto text_box = text_box_.lock();
    if (text_box != nullptr) {
        text_box->ScrollValuesChange(fnBar == SB_HORZ);
    }
    return TRUE;
}


void TextHostBridge::TxInvalidateRect(LPCRECT prc, BOOL fMode) {
    auto text_box = text_box_.lock();
    if (text_box != nullptr) {
        text_box->NeedRepaint();
    }
}


void TextHostBridge::TxViewChange(BOOL fUpdate) {
}


BOOL TextHostBridge::TxCreateCaret(HBITMAP hbmp, INT xWidth, INT yHeight) {

    auto text_box = text_box_.lock();
    if (!text_box) {
        return FALSE;
    }

    auto window = text_box->Window();
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

    if (!text_box->caret_) {
        text_box->caret_ = Create<Caret>(text_box);
    }

    text_box->caret_->SetSize(caret_size);
    return TRUE;
}


BOOL TextHostBridge::TxShowCaret(BOOL fShow) {

    auto text_box = text_box_.lock();
    if (!text_box) {
        return FALSE;
    }

    //Sometimes TxShowCaret would be called even when the 
    //text box is unfocused. This would mess up the caret,
    //so only response to this method while focused.
    if (!text_box->IsFocused()) {
        return FALSE;
    }

    if (!text_box->caret_) {
        return FALSE;
    }

    text_box->caret_->SetIsVisible(!!fShow);
    return TRUE;
}


BOOL TextHostBridge::TxSetCaretPos(INT x, INT y) {

    auto text_box = text_box_.lock();
    if (!text_box) {
        return FALSE;
    }

    if (!text_box->caret_) {
        return FALSE;
    }

    //Note: x and y are in window coordinate, while the position of caret is in control content
    //coordinate.

    float dpi = text_box->GetDPI();
    Point caret_position{
        ToDIPs(static_cast<float>(x), dpi),
        ToDIPs(static_cast<float>(y), dpi)
    };

    auto absolute_content_rect = text_box->AbsoluteContentRect();
    caret_position.SubtractOffset(absolute_content_rect.position);

    text_box->caret_->SetPosition(caret_position);
    return TRUE;
}


BOOL TextHostBridge::TxSetTimer(UINT idTimer, UINT uTimeout) {

    timers_[idTimer] = rx::Interval(std::chrono::milliseconds(uTimeout))
        .ObserveOn(Scheduler::Main())
        .Subscribe([this, idTimer](int) {

        auto text_box = text_box_.lock();
        if (text_box) {

            auto text_service = text_box->GetITextServices();
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

    auto text_box = text_box_.lock();
    if (text_box == nullptr) {
        return;
    }

    if (fCapture) {
        text_box->CaptureMouse();
    }
    else {
        text_box->ReleaseMouse();
    }
}


void TextHostBridge::TxSetFocus() {

}


void TextHostBridge::TxSetCursor(HCURSOR hcur, BOOL fText) {
    SetCursor(hcur);
}


BOOL TextHostBridge::TxScreenToClient(LPPOINT lppt) {

    auto text_box = text_box_.lock();
    if (!text_box) {
        return FALSE;
    }

    auto window = text_box->Window();
    if (!window) {
        return FALSE;
    }

    return ScreenToClient(window->Handle(), lppt);
}


BOOL TextHostBridge::TxClientToScreen(LPPOINT lppt) {

    auto text_box = text_box_.lock();
    if (!text_box) {
        return FALSE;
    }

    auto window = text_box->Window();
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

    auto text_box = text_box_.lock();
    if (!text_box) {
        return S_OK;
    }

    //Returns the rect of rich edit in window.
    auto absolute_content_rect = text_box->AbsoluteContentRect();
    absolute_content_rect.position.y += text_box->GetContentVerticalOffset();

    auto pixels_rect = FromDIPs(absolute_content_rect, text_box->GetDPI());
    auto aligned_rect = Align(pixels_rect);

    *prc = aligned_rect.ToRECT();
    return S_OK;
}


HRESULT TextHostBridge::TxGetViewInset(LPRECT prc) {

    *prc = RECT{};

    auto text_box = text_box_.lock();
    if (!text_box) {
        return S_OK;
    }

    auto window = text_box->Window();
    if (!window) {
        return S_OK;
    }

    *prc = FromDIPs(text_box->GetInset(), window->GetDPI()).ToRECT();
    return S_OK;
}


HRESULT TextHostBridge::TxGetCharFormat(const CHARFORMATW** ppCF) {

    auto text_box = text_box_.lock();
    if (text_box != nullptr) {
        *ppCF = &(text_box->character_format_);
        return S_OK;
    }
    else {
        return E_NOTIMPL;
    }
}


HRESULT TextHostBridge::TxGetParaFormat(const PARAFORMAT** ppPF) {

    auto text_box = text_box_.lock();
    if (text_box != nullptr) {
        *ppPF = &(text_box->paragraph_format_);
        return S_OK;
    }
    else {
        return E_NOTIMPL;
    }
}


COLORREF TextHostBridge::TxGetSysColor(int nIndex) {
    return GetSysColor(nIndex);
}


HRESULT TextHostBridge::TxGetBackStyle(TXTBACKSTYLE* pstyle) {
    *pstyle = TXTBACK_TRANSPARENT;
    return S_OK;
}


HRESULT TextHostBridge::TxGetMaxLength(DWORD* plength) {

    auto text_box = text_box_.lock();
    if (text_box) {
        *plength = text_box->GetMaxLength();
    }
    return S_OK;
}


HRESULT TextHostBridge::TxGetScrollBars(DWORD* pdwScrollBar) {

    auto text_box = text_box_.lock();
    if (text_box) {
        *pdwScrollBar = text_box->scroll_bar_property_;
    }
    return S_OK;
}


HRESULT TextHostBridge::TxGetPasswordChar(_Out_ TCHAR* pch) {

    auto text_box = text_box_.lock();
    if (text_box) {
        *pch = text_box->GetPasswordCharacter();
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

    auto text_box = text_box_.lock();
    if (text_box) {
        *pdwBits = dwMask & text_box->property_bits_;
    }
    return S_OK;
}


HRESULT TextHostBridge::TxNotify(DWORD iNotify, void* pv) {

    auto text_box = text_box_.lock();
    if (text_box == nullptr) {
        return S_OK;
    }

    switch (iNotify) {
    case EN_CHANGE: {
        //Single line rich edit doesn't need to reset cached text height on text changed, but multi
        //line rich edit does. Uncomment this line if multi line rich edit supports 
        //ParagraphAlignment.
        //text_box->ResetCachedTextHeight();
        text_box->NotifyTextChanged();
        return S_OK;
    }

    case EN_SELCHANGE: {
        text_box->RaiseSelectionChangedEvent();
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

    auto text_box = text_box_.lock();
    if (text_box == nullptr) {
        return true;
    }

    auto validator = text_box->GetTextValidator();
    if (validator == nullptr) {
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
        return validator(*text_box, new_text);
    }
    else {
        return true;
    }
}


HIMC TextHostBridge::TxImmGetContext() {
    return nullptr;
}


void TextHostBridge::TxImmReleaseContext(HIMC himc) {
}


HRESULT TextHostBridge::TxGetSelectionBarWidth(LONG* lSelBarWidth) {
    *lSelBarWidth = 0;
    return S_OK;
}


std::shared_ptr<Window> TextHostBridge::GetWindow() const {

    auto text_box = text_box_.lock();
    if (text_box == nullptr) {
        return nullptr;
    }

    return text_box->Window();
}


HWND TextHostBridge::GetWindowHandle() const {

    auto window = GetWindow();
    if (window == nullptr) {
        return nullptr;
    }

    return window->Handle();
}

}