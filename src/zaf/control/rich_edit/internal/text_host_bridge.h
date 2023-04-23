#pragma once

#include <Richedit.h>
#include <TextServ.h>
#include <zaf/control/text_box.h>

namespace zaf::rich_edit::internal {

class TextHostBridge : public ITextHost {
public:
    explicit TextHostBridge(const std::weak_ptr<TextBox>& text_box);

    HRESULT QueryInterface(REFIID riid, void** ppvObject) override;
    ULONG AddRef(void) override { return 0; }
    ULONG Release(void) override { return 0; }

    HDC TxGetDC() override;
    INT TxReleaseDC(HDC hdc) override;
    BOOL TxShowScrollBar(INT fnBar, BOOL fShow) override;
    BOOL TxEnableScrollBar(INT fuSBFlags, INT fuArrowflags) override;
    BOOL TxSetScrollRange(INT fnBar, LONG nMinPos, INT nMaxPos, BOOL fRedraw) override;
    BOOL TxSetScrollPos(INT fnBar, INT nPos, BOOL fRedraw) override;
    void TxInvalidateRect(LPCRECT prc, BOOL fMode) override;
    void TxViewChange(BOOL fUpdate) override;
    BOOL TxCreateCaret(HBITMAP hbmp, INT xWidth, INT yHeight) override;
    BOOL TxShowCaret(BOOL fShow) override;
    BOOL TxSetCaretPos(INT x, INT y) override;
    BOOL TxSetTimer(UINT idTimer, UINT uTimeout) override;
    void TxKillTimer(UINT idTimer) override;
    void TxScrollWindowEx(INT dx, INT dy, LPCRECT lprcScroll, LPCRECT lprcClip, HRGN hrgnUpdate, LPRECT lprcUpdate, UINT fuScroll) override;
    void TxSetCapture(BOOL fCapture) override;
    void TxSetFocus() override;
    void TxSetCursor(HCURSOR hcur, BOOL fText) override;
    BOOL TxScreenToClient(LPPOINT lppt) override;
    BOOL TxClientToScreen(LPPOINT lppt) override;
    HRESULT TxActivate(LONG* plOldState) override;
    HRESULT TxDeactivate(LONG lNewState) override;
    HRESULT TxGetClientRect(LPRECT prc) override;
    HRESULT TxGetViewInset(LPRECT prc) override;
    HRESULT TxGetCharFormat(const CHARFORMATW** ppCF) override;
    HRESULT TxGetParaFormat(const PARAFORMAT** ppPF) override;
    COLORREF TxGetSysColor(int nIndex) override;
    HRESULT TxGetBackStyle(TXTBACKSTYLE* pstyle) override;
    HRESULT TxGetMaxLength(DWORD* plength) override;
    HRESULT TxGetScrollBars(DWORD* pdwScrollBar) override;
    HRESULT TxGetPasswordChar(_Out_ TCHAR* pch) override;
    HRESULT TxGetAcceleratorPos(LONG* pcp) override;
    HRESULT TxGetExtent(LPSIZEL lpExtent) override;
    HRESULT OnTxCharFormatChange(const CHARFORMATW* pCF) override;
    HRESULT OnTxParaFormatChange(const PARAFORMAT* pPF) override;
    HRESULT TxGetPropertyBits(DWORD dwMask, DWORD* pdwBits) override;
    HRESULT TxNotify(DWORD iNotify, void* pv) override;
    HIMC TxImmGetContext() override;
    void TxImmReleaseContext(HIMC himc) override;
    HRESULT TxGetSelectionBarWidth(LONG* lSelBarWidth) override;

private:
    std::shared_ptr<zaf::Window> GetWindow() const;
    HWND GetWindowHandle() const;

    bool NotifyProtected(const ENPROTECTED& info) const;

private:
    std::weak_ptr<TextBox> text_box_;
    std::map<UINT, Subscription> timers_;
};

}