#pragma once

#include <atlbase.h>
#include <Richedit.h>
#include <TextServ.h>
#include <zaf/base/event.h>
#include <zaf/control/self_scrolling_control.h>
#include <zaf/control/text_validator.h>
#include <zaf/control/textual_control.h>
#include <zaf/graphic/frame.h>
#include <zaf/graphic/text/text_range.h>

namespace zaf {

/**
 Represents a text box control.   
 */
class TextBox : public TextualControl, public SelfScrollingControl {
public:
    /**
     Type of text change event.
     */
	typedef Event<const std::shared_ptr<TextBox>&> TextChangeEvent;

    /**
     Type of selection change event.
     */
	typedef Event<const std::shared_ptr<TextBox>&> SelectionChangeEvent;

public:
	TextBox();
	~TextBox();

	void Initialize() override;

    /**
     Get the dimensions of the white space inset around the text.

     The insets are passed in HIMETRIC (each HIMETRIC unit corresponds to 0.01 millimeter).
     */
    const Frame GetInset() const;

    /**
     Set the dimensions of the white space inset around the text.
     */
    void SetInset(const Frame& inset);

    /**
     Set the dimensions of the white space inset around the text.

     This overload is a shortcut to set all direction insets.
     */
    void SetInset(float inset) {
        SetInset(Frame(inset, inset, inset, inset));
    }

    /**
     Get the maximum length of text. 

     The default value is the maximum of std::uint32_t.
     */
	std::uint32_t GetMaximumLength() const;

    /**
     Set the maximum length of text. 
     */
	void SetMaximumLength(std::uint32_t max_length);

    /**
     Get a value indicating that whether all characters is shown as password character.

     The default value is false.
     */
	bool UsePasswordCharacter() const;
    
    /**
     Set a value indicating that whether all characters is shown as password character.
     */
	void SetUsePasswordCharacter(bool use_password_char);

    /**
     Get the password character.

     The default character is *.
     */
	wchar_t GetPasswordCharacter() const;

    /**
     Set the password character.
     */
	void SetPasswordCharacter(wchar_t password_char);

    /**
     Get a value indicating that whether the text box is multiline.

     The default value is false.
     */
	bool IsMultiline() const;

    /**
     Set value indicating that whether the text box is multiline.
     */
	void SetIsMultiline(bool is_multiline);

    /**
     Get a value indicating that whether the text box is read only.

     The default value is falsel
     */
	bool IsReadOnly() const;

    /**
     Set a value indicating that whether the text box is read only.
     */
	void SetIsReadOnly(bool is_read_only);

    /**
     Get a value indicating that whether the text box allows beep when an
     invalid character is inputted.

     The default value is true.
     */
	bool AllowBeep() const;

    /**
     Set a value indicating that whether the text box allows beep when an
     invalid character is inputted.
     */
	void SetAllowBeep(bool allow_beep);

    /**
     Get the slected text range.
     */
	const TextRange GetSelectionRange() const;

    /**
     Set the slected text range.
     */
	void SetSelectionRange(const TextRange& range);

	const std::wstring GetText() const override;
	void SetText(const std::wstring& text) override;

    /**
     Get text validator.

     The default validator is nullptr, means the text box accepts any
     inputted text.
     */
    const TextValidator GetTextValidator() const;

    /**
     Set the text validator.
     */
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

    /**
     Get text change event.

     This event is raised when the text is changed.
     */
	TextChangeEvent::Proxy GetTextChangeEvent();

    /**
     Get selection change event.

     This event is raise when selected text is changed.
     */
	SelectionChangeEvent::Proxy GetSelectionChangeEvent();
    ScrollBarChangeEvent::Proxy GetScrollBarChangeEvent() override;
    ScrollValuesChangeEvent::Proxy GetScrollValuesChangeEvent() override;

    bool AcceptKeyMessage(const KeyMessage& message) const override;

    /**
     Get a value indicating that whether the text box accepts TAB character when the
     TAB key is pressed.

     The default value is false.
     */
    bool AcceptTab() const;

    /**
     Set a value indicating that whether the text box accepts TAB character when the
     TAB key is pressed.
     */
    void SetAcceptTab(bool accept_tab);

    /**
     Get a value indicating that whether the text box accepts return character when the
     return key is pressed.

     The default value is false.
    */
    bool AcceptReturn() const;

    /**
     Set a value indicating that whether the text box accepts return character when the
     return key is pressed.
     */
    void SetAcceptReturn(bool accept_return);

    /**
     Determine whether the text box can be undone.
     */
    bool CanUndo() const;

    /**
     Undo the last operation.

     @return
        Return a value indicating that whether the undoing succeeds.
     */
    bool Undo();

    /**
     Determine whether the text box can be redone.
     */
    bool CanRedo() const;

    /**
     Redo the last undone operation.

     @return 
        Return a value indicating that whether the redoing succeeds.
     */
    bool Redo();

    /**
     Scroll up by one line.
     */
    void ScrollUpByLine();

    /**
     Scroll down by one line.
     */
    void ScrollDownByLine();

    /**
     Scroll up by one page.
     */
    void ScrollUpByPage();

    /**
     Scroll down by one page.
     */
    void ScrollDownByPage();

    /**
     Scroll up to the begin.
     */
    void ScrollUpToBegin();

    /**
     Scroll down to the end.
     */
    void ScrollDownToEnd();

    /**
     Scroll left to the begin.
     */
    void ScrollLeftToBegin();

    /**
     Scroll right to the end.
     */
    void ScrollRightToEnd();

    void VerticallyScroll(int new_value) override;
    void HorizontallyScroll(int new_value) override;

protected:
    void Repaint(Canvas& canvas, const Rect& dirty_rect) override;
    const Rect GetTextRect() const override;
	void Layout(const Rect& previous_rect) override;

	void ChangeMouseCursor(const Message& message, bool& is_changed) override;
    void MouseMove(const Point& position, const MouseMessage& message) override;
    void MouseDown(const Point& position, const MouseMessage& message) override;
    void MouseUp(const Point& position, const MouseMessage& message) override;

    bool KeyDown(const KeyMessage& message) override;
    bool KeyUp(const KeyMessage& message) override;
    bool CharInput(const CharMessage& message) override;

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
    float GetPaintContentOffset(HDC hdc);
    void ResetRequiredHeight();
	const Rect GetAbsoluteContentRect() const;
	bool ChangeMouseCursor();
	bool HasPropertyBit(DWORD bit) const;
	void ChangePropertyBit(DWORD bit, bool is_set);
    void ChangeScrollBarPropertyBits(DWORD bits, bool is_set);
    void GetScrollValues(bool is_horizontal, int& current_value, int& min_value, int& max_value);
    void SendScrollMessage(bool is_horizontal, WORD scroll_type);
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
    float required_height_;
};

}