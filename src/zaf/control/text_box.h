#pragma once

#include <atlbase.h>
#include <Richedit.h>
#include <TextServ.h>
#include <zaf/base/event.h>
#include <zaf/control/self_scrolling_control.h>
#include <zaf/control/text_validator.h>
#include <zaf/control/textual_control.h>
#include <zaf/graphic/text/text_range.h>

namespace zaf {

class TextBox : public TextualControl, public SelfScrollingControl {
public:
	typedef Event<const std::shared_ptr<TextBox>&> TextChangeEvent;
	typedef Event<const std::shared_ptr<TextBox>&> SelectionChangeEvent;

public:
	TextBox();
	~TextBox();

	void Initialize() override;

	std::uint32_t GetMaximumLength() const;
	void SetMaximumLength(std::uint32_t max_length);

	bool UsePasswordCharacter() const;
	void SetUsePasswordCharacter(bool use_password_char);

	wchar_t GetPasswordCharacter() const;
	void SetPasswordCharacter(wchar_t password_char);

	bool IsMultiline() const;
	void SetIsMultiline(bool is_multiline);

	bool IsReadOnly() const;
	void SetIsReadOnly(bool is_read_only);

	bool AllowBeep() const;
	void SetAllowBeep(bool allow_beep);

	const TextRange GetSelectionRange() const;
	void SetSelectionRange(const TextRange& range);

	const std::wstring GetText() const override;
	void SetText(const std::wstring& text) override;

    const TextValidator GetTextValidator() const;
    void SetTextValidator(const TextValidator& validator);

	const Font GetFont() const override;
	void SetFont(const Font& font) override;

	TextAlignment GetTextAlignment() const override;
	void SetTextAlignment(TextAlignment alignment) override;

	WordWrapping GetWordWrapping() const override;
	void SetWordWrapping(WordWrapping word_wrapping) override;

    void SetAllowVerticalScroll(bool allow) override;
    void SetAllowHorizontalScroll(bool allow) override;

    void SetAutoHideScrollBars(bool auto_hide) override;

    bool CanShowVerticalScrollBar() override;
    bool CanShowHorizontalScrollBar() override;

    bool CanEnableVerticalScrollBar() override;
    bool CanEnableHorizontalScrollBar() override;

    void GetVerticalScrollValues(int& current_value, int& min_value, int& max_value) override;
    void GetHorizontalScrollValues(int& current_value, int& min_value, int& max_value) override;

	TextChangeEvent::Proxy GetTextChangeEvent();
	SelectionChangeEvent::Proxy GetSelectionChangeEvent();
    ScrollBarChangeEvent::Proxy GetScrollBarChangeEvent() override;
    ScrollValuesChangeEvent::Proxy GetScrollValuesChangeEvent() override;

    bool CanUndo() const;
    bool Undo();

    bool CanRedo() const;
    bool Redo();

    void ScrollUpByLine();
    void ScrollDownByLine();

    void ScrollUpByPage();
    void ScrollDownByPage();

    void ScrollToBegin();
    void ScrollToEnd();

    void VerticallyScroll(int new_value) override;
    void HorizontallyScroll(int new_value) override;

protected:
    void Repaint(Canvas& canvas, const Rect& dirty_rect) override;
    const Rect GetTextRect() const override;
	void Layout(const Rect& previous_rect) override;

	void ChangeMouseCursor(const Message& message, bool& is_changed) override;
    void MouseMove(const MouseMessage& message) override;
    void MouseDown(const MouseMessage& message) override;
    void MouseUp(const MouseMessage& message) override;

    void KeyDown(const Message& message) override;
    void KeyUp(const Message& message) override;
    void CharInput(const Message& message) override;

	void FocusGain() override;
	void FocusLose() override;

private:
	class TextHostBridge : public ITextHost {
	public:
		TextHostBridge(const std::shared_ptr<TextBox>& text_box);

		HRESULT __stdcall QueryInterface(REFIID riid, void** ppvObject) override;
		ULONG __stdcall AddRef(void) override { return 0; }
		ULONG  __stdcall Release(void) override { return 0; }
		HDC TxGetDC() override;
		INT	TxReleaseDC(HDC hdc) override;
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
		HRESULT	TxActivate(LONG * plOldState) override;
		HRESULT	TxDeactivate(LONG lNewState) override;
		HRESULT	TxGetClientRect(LPRECT prc) override;
		HRESULT	TxGetViewInset(LPRECT prc) override;
		HRESULT	TxGetCharFormat(const CHARFORMATW **ppCF) override;
		HRESULT	TxGetParaFormat(const PARAFORMAT **ppPF) override;
		COLORREF TxGetSysColor(int nIndex) override;
		HRESULT	TxGetBackStyle(TXTBACKSTYLE *pstyle) override;
		HRESULT	TxGetMaxLength(DWORD *plength) override;
		HRESULT	TxGetScrollBars(DWORD *pdwScrollBar) override;
		HRESULT	TxGetPasswordChar(_Out_ TCHAR *pch) override;
		HRESULT	TxGetAcceleratorPos(LONG *pcp) override;
		HRESULT	TxGetExtent(LPSIZEL lpExtent) override;
		HRESULT	OnTxCharFormatChange(const CHARFORMATW * pCF) override;
		HRESULT	OnTxParaFormatChange(const PARAFORMAT * pPF) override;
		HRESULT	TxGetPropertyBits(DWORD dwMask, DWORD *pdwBits) override;
		HRESULT	TxNotify(DWORD iNotify, void *pv) override;
		HIMC TxImmGetContext() override;
		void TxImmReleaseContext(HIMC himc) override;
		HRESULT	TxGetSelectionBarWidth(LONG *lSelBarWidth) override;

	private:
		std::shared_ptr<Window> GetWindow() const;
		HWND GetWindowHandle() const;

        bool NotifyProtected(const ENPROTECTED& info) const;

	private:
		std::weak_ptr<TextBox> text_box_;
	};

private:
	void InitializeTextService();
	const Rect GetAbsoluteContentRect() const;
	bool ChangeMouseCursor();
	bool HasPropertyBit(DWORD bit) const;
	void ChangePropertyBit(DWORD bit, bool is_set);
    void ChangeScrollBarPropertyBits(DWORD bits, bool is_set);
    void GetScrollValues(bool is_horizontal, int& current_value, int& min_value, int& max_value);
    void SendScrollMessage(WORD scroll_type);
    void Scroll(bool is_horizontal, int new_value);
    void ScrollBarChange();
    void ScrollValuesChange(bool is_horizontal);

private:
	std::shared_ptr<TextHostBridge> text_host_bridge_;
	CComPtr<ITextServices> text_service_;
	DWORD property_bits_;
	CHARFORMATW character_format_;
	PARAFORMAT paragraph_format_;
    DWORD scroll_bar_property_;
};

}