#include <zaf/text_box.h>
#include <cassert>
#include <zaf/canvas.h>
#include <zaf/caret.h>
#include <zaf/renderer.h>
#include <zaf/window.h>
#include <zaf/internal/conversion.h>
#include <zaf/internal/log.h>

EXTERN_C const IID IID_ITextServices = {
	0x8d33f740,
	0xcf58,
	0x11ce,
	{ 0xa8, 0x9d, 0x00, 0xaa, 0x00, 0x6c, 0xad, 0xc5 }
};

EXTERN_C const IID IID_ITextHost = {
	0xc5bdd8d0,
	0xd26e,
	0x11ce,
	{ 0xa8, 0x9e, 0x00, 0xaa, 0x00, 0x6c, 0xad, 0xc5 }
};

namespace zaf {

TextBox::TextBox() :
	not_initialized_state_(std::make_unique<NotInitializedState>()) {

	SetCanFocused(true);
}


TextBox::~TextBox() {

}


void TextBox::Initialize() {

	text_host_ = std::make_shared<TextHost>(
		std::dynamic_pointer_cast<TextBox>(shared_from_this()),
		not_initialized_state_->property_bits
	);

	HMODULE module_handle = LoadLibrary(L"riched20.dll");
	if (module_handle == nullptr) {
		return;
	}
		
	typedef HRESULT(_stdcall*CreateTextServicesFunction)(IUnknown*, ITextHost*, IUnknown**);
	CreateTextServicesFunction create_text_services = reinterpret_cast<CreateTextServicesFunction>(
		GetProcAddress(module_handle, "CreateTextServices")
	);
	if (create_text_services == nullptr) {
		return;
	}

 	CComPtr<IUnknown> service;
	HRESULT result = create_text_services(nullptr, text_host_.get(), &service);
	if (FAILED(result)) {
		return;
	}

	service->QueryInterface(IID_ITextServices, reinterpret_cast<void**>(&text_service_));
	if (text_service_ == nullptr) {
		return;
	}

	text_service_->OnTxInPlaceActivate(nullptr);
	text_service_->TxSetText(not_initialized_state_->text.c_str());

	not_initialized_state_ = nullptr;
}


void TextBox::Layout(const Rect& previous_rect) {

	if (text_service_ == nullptr) {
		return;
	}

	text_service_->OnTxPropertyBitsChange(TXTBIT_CLIENTRECTCHANGE, TXTBIT_CLIENTRECTCHANGE);
}


void TextBox::Repaint(Canvas& canvas, const Rect& dirty_rect) {

	if (text_service_ == nullptr) {
		return;
	}

	auto parent = GetParent();
	if (parent == nullptr) {
		return;
	}

	auto render_target = canvas.GetRenderer()->GetHandle();
	CComPtr<ID2D1GdiInteropRenderTarget> gdi_interop_render_target;
	render_target->QueryInterface(IID_ID2D1GdiInteropRenderTarget, reinterpret_cast<void**>(&gdi_interop_render_target));

	HDC hdc = nullptr;
	HRESULT result = gdi_interop_render_target->GetDC(D2D1_DC_INITIALIZE_MODE_COPY, &hdc);
	if (FAILED(result)) {
		return;
	}

	BeginPath(hdc);
	RECT path_rect = ToWin32Rect(canvas.GetAbsoluteDirtyRect());
	//Clip path not include the tail edges of rectangle, 
	//so increase the right and bottom values.
	path_rect.right += 1;
	path_rect.bottom += 1;
	Rectangle(hdc, path_rect.left, path_rect.top, path_rect.right, path_rect.bottom);
	EndPath(hdc);
	SelectClipPath(hdc, RGN_COPY);

	RECT win32_absolute_rect = ToWin32Rect(canvas.GetAbsoluteRect());

	text_service_->TxDraw(
		DVASPECT_CONTENT,
		0,
		nullptr,
		nullptr,
		hdc,
		nullptr,
		reinterpret_cast<LPCRECTL>(&win32_absolute_rect),
		nullptr,
		&win32_absolute_rect,
		nullptr,
		0,
		0
	);

	gdi_interop_render_target->ReleaseDC(nullptr);

	Control::Repaint(canvas, dirty_rect);
}


void TextBox::Paint(Canvas& canvas, const Rect& dirty_rect) {

}


void TextBox::ChangeMouseCursor(WPARAM wParam, LPARAM lParam, bool& is_changed) {

	if (ChangeMouseCursor()) {
		is_changed = true;
	}
	else {
		Control::ChangeMouseCursor(wParam, lParam, is_changed);
	}
}


bool TextBox::ChangeMouseCursor() {

	if (text_service_ == nullptr) {
		return false;
	}

	auto window = GetWindow();
	if (window == nullptr) {
		return false;
	}

	Point mouse_position = window->GetMousePosition();

	HRESULT result = text_service_->OnTxSetCursor(
		DVASPECT_CONTENT,
		0,
		nullptr,
		nullptr,
		nullptr,
		nullptr,
		nullptr,
		mouse_position.x,
		mouse_position.y
	);

	if (FAILED(result)) {
		return false;
	}

	return true;
}


void TextBox::MouseMove(const Point& position, WPARAM wParam, LPARAM lParam) {
	if (text_service_ != nullptr) {
		text_service_->TxSendMessage(WM_MOUSEMOVE, wParam, lParam, nullptr);
	}
}


void TextBox::MouseDown(const Point& position, MouseButton button, WPARAM wParam, LPARAM lParam) {
	SetIsFocused(true);
	if (text_service_ != nullptr) {
		text_service_->TxSendMessage(WM_LBUTTONDOWN, wParam, lParam, nullptr);
	}
}


void TextBox::MouseUp(const Point& position, MouseButton button, WPARAM wParam, LPARAM lParam) {
	if (text_service_ != nullptr) {
		text_service_->TxSendMessage(WM_LBUTTONUP, wParam, lParam, nullptr);
	}
}


void TextBox::KeyDown(WPARAM wParam, LPARAM lParam) {
	if (text_service_ != nullptr) {
		text_service_->TxSendMessage(WM_KEYDOWN, wParam, lParam, nullptr);
	}
}


void TextBox::KeyUp(WPARAM wParam, LPARAM lParam) {
	if (text_service_ != nullptr) {
		text_service_->TxSendMessage(WM_KEYUP, wParam, lParam, nullptr);
	}
}


void TextBox::CharInput(WPARAM wParam, LPARAM lParam) {
	if (text_service_ != nullptr) {
		text_service_->TxSendMessage(WM_CHAR, wParam, lParam, nullptr);
	}
}


void TextBox::FocusGain() {

	if (text_service_ != nullptr) {
		text_service_->TxSendMessage(WM_SETFOCUS, 0, 0, nullptr);
	}
}


void TextBox::FocusLose() {

	auto window = GetWindow();
	if (window != nullptr) {
		window->GetCaret()->Hide();
	}

	if (text_service_ != nullptr) {
		text_service_->TxSendMessage(WM_KILLFOCUS, 0, 0, nullptr);
	}
}


bool TextBox::IsReadOnly() const {
	return HasPropertyBit(TXTBIT_READONLY);
}


void TextBox::SetIsReadOnly(bool is_read_only) {
	ChangePropertyBit(TXTBIT_READONLY, is_read_only);
}


const std::wstring TextBox::GetText() const {

	if (not_initialized_state_ != nullptr) {
		return not_initialized_state_->text;
	}

	std::wstring text;
	if (text_service_ != nullptr) {

		BSTR text_buffer = nullptr;
		HRESULT result = text_service_->TxGetText(&text_buffer);
		if (SUCCEEDED(result) && text_buffer != nullptr) {
		
			text.assign(text_buffer);
			SysFreeString(text_buffer);
		}
	}

	return text;
}


void TextBox::SetText(const std::wstring& text) {

	if (not_initialized_state_ != nullptr) {
		not_initialized_state_->text = text;
	}
	else {
		if (text_service_ != nullptr) {
			text_service_->TxSetText(text.c_str());
		}
	}
}


bool TextBox::HasPropertyBit(DWORD bit) const {

	DWORD property_bits = 0;

	if (not_initialized_state_ != nullptr) {
		property_bits = not_initialized_state_->property_bits;
	}
	else {
		if (text_host_ != nullptr) {
			property_bits = text_host_->property_bits;
		}
	}

	return (property_bits & bit) != 0;
}


void TextBox::ChangePropertyBit(DWORD bit, bool is_set) {

	DWORD* property_bits = nullptr;

	if (not_initialized_state_ != nullptr) {
		property_bits = &not_initialized_state_->property_bits;
	}
	else {
		if (text_host_ != nullptr) {
			property_bits = &text_host_->property_bits;
		}	
	}

	if (property_bits != nullptr) {

		if (is_set) {
			*property_bits |= bit;
		}
		else {
			*property_bits &= ~bit;
		}

		if (text_service_ != nullptr) {
			text_service_->OnTxPropertyBitsChange(bit, *property_bits);
		}
	}
}


TextBox::TextHost::TextHost(const std::shared_ptr<TextBox>& text_box, DWORD property_bits) :
	text_box_(text_box),
	property_bits(property_bits) {

}


HRESULT TextBox::TextHost::QueryInterface(REFIID riid, void** ppvObject) {

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


HDC TextBox::TextHost::TxGetDC() {
	return GetDC(GetWindowHandle());
}


INT TextBox::TextHost::TxReleaseDC(HDC hdc) {
	return ReleaseDC(GetWindowHandle(), hdc);
}


BOOL TextBox::TextHost::TxShowScrollBar(INT fnBar, BOOL fShow) {
	return FALSE;
}


BOOL TextBox::TextHost::TxEnableScrollBar(INT fuSBFlags, INT fuArrowflags) {
	return FALSE;
}


BOOL TextBox::TextHost::TxSetScrollRange(INT fnBar, LONG nMinPos, INT nMaxPos, BOOL fRedraw) {
	return FALSE;
}


BOOL TextBox::TextHost::TxSetScrollPos(INT fnBar, INT nPos, BOOL fRedraw) {
	return FALSE;
}


void TextBox::TextHost::TxInvalidateRect(LPCRECT prc, BOOL fMode) {
	auto text_box = text_box_.lock();
	if (text_box != nullptr) {
		text_box->NeedRepaint();
	}
}


void TextBox::TextHost::TxViewChange(BOOL fUpdate) {
}


BOOL TextBox::TextHost::TxCreateCaret(HBITMAP hbmp, INT xWidth, INT yHeight) {

	//LOG() << "CreateCaret: " << hbmp << ',' << xWidth << ',' << yHeight;
	//return CreateCaret(GetWindowHandle(), hbmp, xWidth, yHeight);

	auto window = GetWindow();
	if (window == nullptr) {
		return FALSE;
	}

	window->GetCaret()->SetSize(Size(xWidth, yHeight));
	return TRUE;
}


BOOL TextBox::TextHost::TxShowCaret(BOOL fShow) {

	/*
	LOG() << "ShowCaret: " << fShow;
	if (fShow) {
		return ShowCaret(GetWindowHandle());
	}
	else {
		return HideCaret(GetWindowHandle());
	}
	*/

	auto window = GetWindow();
	if (window == nullptr) {
		return FALSE;
	}
	
	const auto& caret = window->GetCaret();
	if (fShow) {
		caret->Show();
	}
	else {
		caret->Hide();
	}
	return TRUE;
}


BOOL TextBox::TextHost::TxSetCaretPos(INT x, INT y) {

	//LOG() << "SetCaretPos: " << x << ',' << y;
	//return SetCaretPos(x, y);

	auto window = GetWindow();
	if (window == nullptr) {
		return FALSE;
	}

	const auto& caret = window->GetCaret();
	caret->SetPosition(Point(x, y));
	return TRUE;
}


BOOL TextBox::TextHost::TxSetTimer(UINT idTimer, UINT uTimeout) {
	return FALSE;
}


void TextBox::TextHost::TxKillTimer(UINT idTimer) {
}


void TextBox::TextHost::TxScrollWindowEx(INT dx, INT dy, LPCRECT lprcScroll, LPCRECT lprcClip, HRGN hrgnUpdate, LPRECT lprcUpdate, UINT fuScroll) {
}


void TextBox::TextHost::TxSetCapture(BOOL fCapture) {

	auto text_box = text_box_.lock();
	if (text_box == nullptr) {
		return;
	}

	text_box->NeedCaptureMouse(fCapture == TRUE);
}


void TextBox::TextHost::TxSetFocus() {
	
}


void TextBox::TextHost::TxSetCursor(HCURSOR hcur, BOOL fText) {
	SetCursor(hcur);
}


BOOL TextBox::TextHost::TxScreenToClient(LPPOINT lppt) {
	return FALSE;
}


BOOL TextBox::TextHost::TxClientToScreen(LPPOINT lppt) {
	return FALSE;
}


HRESULT TextBox::TextHost::TxActivate(LONG * plOldState) {
	return E_NOTIMPL;
}


HRESULT TextBox::TextHost::TxDeactivate(LONG lNewState) {
	return E_NOTIMPL;
}


HRESULT TextBox::TextHost::TxGetClientRect(LPRECT prc) {

	Rect rect;

	auto text_box = text_box_.lock();
	if (text_box != nullptr) {
		rect = text_box->GetAbsoluteRect();	
	}

	*prc = ToWin32Rect(rect);
	return S_OK;
}


HRESULT TextBox::TextHost::TxGetViewInset(LPRECT prc) {
	*prc = RECT();
	return S_OK;
}


HRESULT TextBox::TextHost::TxGetCharFormat(const CHARFORMATW **ppCF) {

	if (char_format_ == nullptr) {
		char_format_ = std::make_unique<CHARFORMATW>();
		char_format_->cbSize = sizeof(CHARFORMATW);
		/*
		char_format_->dwMask = CFM_SIZE | CFM_COLOR | CFM_FACE | CFM_CHARSET | CFM_OFFSET;
		char_format_->yHeight = 500;
		char_format_->yOffset = 0;
		//char_format_->dwEffects = CFE_AUTOCOLOR;
		char_format_->crTextColor = RGB(1, 1, 1);
		char_format_->bCharSet = DEFAULT_CHARSET;
		char_format_->bPitchAndFamily = DEFAULT_PITCH;
		wcscpy_s(char_format_->szFaceName, L"Î¢ÈíÑÅºÚ");
		*/
	}

	*ppCF = char_format_.get();
	return S_OK;
}


HRESULT TextBox::TextHost::TxGetParaFormat(const PARAFORMAT **ppPF) {

	if (para_format_ == nullptr) {
		para_format_ = std::make_unique<PARAFORMAT>();
		para_format_->cbSize = sizeof(PARAFORMAT);
		/*
		para_format_->dwMask = PFM_ALL;
		para_format_->wAlignment = PFA_LEFT;
		para_format_->cTabCount = 1;
		para_format_->rgxTabs[0] = lDefaultTab;
		*/
	}

	*ppPF = para_format_.get();
	return S_OK;
}


COLORREF TextBox::TextHost::TxGetSysColor(int nIndex) {
	return GetSysColor(nIndex);
}


HRESULT TextBox::TextHost::TxGetBackStyle(TXTBACKSTYLE *pstyle) {
	*pstyle = TXTBACK_OPAQUE;
	return S_OK;
}


HRESULT TextBox::TextHost::TxGetMaxLength(DWORD *plength) {
	*plength = INFINITE;
	return S_OK;
}


HRESULT TextBox::TextHost::TxGetScrollBars(DWORD *pdwScrollBar) {
	return E_NOTIMPL;
}


HRESULT TextBox::TextHost::TxGetPasswordChar(_Out_ TCHAR *pch) {
	return E_NOTIMPL;
}


HRESULT TextBox::TextHost::TxGetAcceleratorPos(LONG *pcp) {
	return E_NOTIMPL;
}


HRESULT TextBox::TextHost::TxGetExtent(LPSIZEL lpExtent) {
	return E_NOTIMPL;
}


HRESULT TextBox::TextHost::OnTxCharFormatChange(const CHARFORMATW * pCF) {
	return E_NOTIMPL;
}


HRESULT TextBox::TextHost::OnTxParaFormatChange(const PARAFORMAT * pPF) {
	return E_NOTIMPL;
}


HRESULT TextBox::TextHost::TxGetPropertyBits(DWORD dwMask, DWORD *pdwBits) {
	*pdwBits = dwMask & property_bits;
	return S_OK;
}


HRESULT TextBox::TextHost::TxNotify(DWORD iNotify, void *pv) {
	return S_OK;
}


HIMC TextBox::TextHost::TxImmGetContext() {
	return nullptr;
}


void TextBox::TextHost::TxImmReleaseContext(HIMC himc) {
}


HRESULT TextBox::TextHost::TxGetSelectionBarWidth(LONG *lSelBarWidth) {
	*lSelBarWidth = 0;
	return S_OK;
}


std::shared_ptr<Window> TextBox::TextHost::GetWindow() const {

	auto text_box = text_box_.lock();
	if (text_box == nullptr) {
		return nullptr;
	}

	return text_box->GetWindow();
}


HWND TextBox::TextHost::GetWindowHandle() const {

	auto window = GetWindow();
	if (window == nullptr) {
		return nullptr;
	}

	return window->GetHandle();
}


TextBox::NotInitializedState::NotInitializedState() :
	property_bits(TXTBIT_ALLOWBEEP) {

}

}