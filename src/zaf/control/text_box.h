#pragma once

#include <atlbase.h>
#include <Richedit.h>
#include <TextServ.h>
#include <zaf/base/event.h>
#include <zaf/control/control.h>

namespace zaf {

class TextBox : public Control {
public:
	typedef Event<const std::shared_ptr<TextBox>&> TextChangeEvent;

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

	const std::wstring GetText() const override;
	void SetText(const std::wstring& text) override;

	TextChangeEvent::Proxy GetTextChangeEvent();

protected:
	void Layout(const Rect& previous_rect) override;
	void Repaint(Canvas& canvas, const Rect& dirty_rect) override;
	void Paint(Canvas& canvas, const Rect& dirty_rect) override;

	void ChangeMouseCursor(WPARAM wParam, LPARAM lParam, bool& is_changed) override;
	void MouseMove(const Point& position, WPARAM wParam, LPARAM lParam) override;
	void MouseDown(const Point& position, MouseButton button, WPARAM wParam, LPARAM lParam) override;
	void MouseUp(const Point& position, MouseButton button, WPARAM wParam, LPARAM lParam) override;

	void KeyDown(WPARAM wParam, LPARAM lParam) override;
	void KeyUp(WPARAM wParam, LPARAM lParam) override;
	void CharInput(WPARAM wParam, LPARAM lParam) override;

	void FocusGain() override;
	void FocusLose() override;

private:
	class TextHost : public ITextHost {
	public:
		TextHost(const std::shared_ptr<TextBox>& text_box, DWORD property_bits);

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

	public:
		DWORD property_bits;

	private:
		std::shared_ptr<Window> GetWindow() const;
		HWND GetWindowHandle() const;

	private:
		std::weak_ptr<TextBox> text_box_;
		std::unique_ptr<CHARFORMATW> char_format_;
		std::unique_ptr<PARAFORMAT> para_format_;
	};

	class NotInitializedState {
	public:
		NotInitializedState();

		DWORD property_bits;
		std::wstring text;
		std::uint32_t max_length;
		wchar_t password_char;
	};

private:
	const Rect GetAbsoluteContentRect() const;
	bool ChangeMouseCursor();
	bool HasPropertyBit(DWORD bit) const;
	void ChangePropertyBit(DWORD bit, bool is_set);

private:
	std::shared_ptr<TextHost> text_host_;
	CComPtr<ITextServices> text_service_;
	std::unique_ptr<NotInitializedState> not_initialized_state_;
};

}