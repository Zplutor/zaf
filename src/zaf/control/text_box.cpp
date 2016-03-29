#include <zaf/control/text_box.h>
#include <cassert>
#include <zaf/base/log.h>
#include <zaf/graphic/canvas.h>
#include <zaf/graphic/renderer.h>
#include <zaf/graphic/font/font.h>
#include <zaf/window/caret.h>
#include <zaf/window/window.h>

#undef max

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

static const wchar_t* const kMaximumLengthPropertyName = L"MaximumLength";
static const wchar_t* const kPasswordCharacterPropertyName = L"PasswordCharacter";
static const wchar_t* const kTextChangeEventPropertyName = L"TextChangeEvent";

static const std::uint32_t kDefaultMaximumLength = std::numeric_limits<std::uint32_t>::max();
static const wchar_t kDefaultPasswordCharacter = L'*';

namespace zaf {

TextBox::TextBox() :
	not_initialized_state_(std::make_unique<NotInitializedState>()) {

	SetCanFocused(true);
}


TextBox::~TextBox() {

}


void TextBox::Initialize() {

	__super::Initialize();

	SetBorderWidth(1);
	SetColor(PaintComponent::Border, PaintState::Normal, Color::Black);

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
	text_service_->TxSendMessage(EM_SETEVENTMASK, 0, ENM_CHANGE, nullptr);

	not_initialized_state_ = nullptr;
}


void TextBox::Layout(const Rect& previous_rect) {

	if (text_service_ == nullptr) {
		return;
	}

	text_service_->OnTxPropertyBitsChange(TXTBIT_CLIENTRECTCHANGE, TXTBIT_CLIENTRECTCHANGE);
}


void TextBox::Repaint(Canvas& canvas, const Rect& dirty_rect) {

	__super::Repaint(canvas, dirty_rect);

	if (text_service_ == nullptr) {
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
	RECT path_rect = canvas.GetAbsolutePaintableRect().ToRECT();
	//Clip path not include the tail edges of rectangle, 
	//so increase the right and bottom values.
	path_rect.right += 1;
	path_rect.bottom += 1;
	Rectangle(hdc, path_rect.left, path_rect.top, path_rect.right, path_rect.bottom);
	EndPath(hdc);
	SelectClipPath(hdc, RGN_COPY);

	Rect absolute_rect = GetAbsoluteContentRect();

	RECT win32_absolute_rect = absolute_rect.ToRECT();
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
}


void TextBox::Paint(Canvas& canvas, const Rect& dirty_rect) {

	__super::Paint(canvas, dirty_rect);
}


const Rect TextBox::GetAbsoluteContentRect() const {

	auto rect = GetAbsoluteRect();
	rect.Inflate(-GetBorderWidth());
	return rect;
}


std::uint32_t TextBox::GetMaximumLength() const {

	if (not_initialized_state_ != nullptr) {
		return not_initialized_state_->max_length;
	}

	auto max_length = GetPropertyMap().TryGetProperty<std::uint32_t>(kMaximumLengthPropertyName);
	if (max_length != nullptr) {
		return *max_length;
	}
	else {
		return kDefaultMaximumLength;
	}
}


void TextBox::SetMaximumLength(std::uint32_t max_length) {

	if (not_initialized_state_ != nullptr) {
		not_initialized_state_->max_length = max_length;
	}
	else {

		GetPropertyMap().SetProperty(kMaximumLengthPropertyName, max_length);

		if (text_service_ != nullptr) {
			text_service_->OnTxPropertyBitsChange(TXTBIT_MAXLENGTHCHANGE, TXTBIT_MAXLENGTHCHANGE);
		}
	}
}


bool TextBox::UsePasswordCharacter() const {
	return HasPropertyBit(TXTBIT_USEPASSWORD);
}

void TextBox::SetUsePasswordCharacter(bool use_password_char) {
	ChangePropertyBit(TXTBIT_USEPASSWORD, use_password_char);
}


wchar_t TextBox::GetPasswordCharacter() const {
	
	if (not_initialized_state_ != nullptr) {
		return not_initialized_state_->password_char;
	}

	auto password_char = GetPropertyMap().TryGetProperty<wchar_t>(kPasswordCharacterPropertyName);
	if (password_char != nullptr) {
		return *password_char;
	}
	else {
		return kDefaultPasswordCharacter;
	}
}

void TextBox::SetPasswordCharacter(wchar_t password_char) {

	if (not_initialized_state_ != nullptr) {
		not_initialized_state_->password_char = password_char;
	}
	else {

		GetPropertyMap().SetProperty(kPasswordCharacterPropertyName, password_char);

		if ((text_service_ != nullptr) && UsePasswordCharacter()) {
			text_service_->OnTxPropertyBitsChange(TXTBIT_USEPASSWORD, TXTBIT_USEPASSWORD);
		}
	}
}


bool TextBox::IsMultiline() const {
	return HasPropertyBit(TXTBIT_MULTILINE);
}

void TextBox::SetIsMultiline(bool is_multiline) {
	ChangePropertyBit(TXTBIT_MULTILINE, is_multiline);
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


TextBox::TextChangeEvent::Proxy TextBox::GetTextChangeEvent() {

	auto& event = GetPropertyMap().GetProperty<TextChangeEvent>(kTextChangeEventPropertyName);
	return TextChangeEvent::Proxy(event);
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

	auto window = GetWindow();
	if (window == nullptr) {
		return FALSE;
	}

	window->GetCaret()->SetSize(Size(xWidth, yHeight));
	return TRUE;
}


BOOL TextBox::TextHost::TxShowCaret(BOOL fShow) {

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
		rect = text_box->GetAbsoluteContentRect();
	}

	*prc = rect.ToRECT();
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
		char_format_->dwMask = 0;

		auto text_box = text_box_.lock();
		if (text_box != nullptr) { 

			auto font = text_box->GetFont();

			char_format_->dwMask |= CFM_FACE;
			wcscpy_s(char_format_->szFaceName, font.family_name.c_str());

			char_format_->dwMask |= CFM_SIZE;
			char_format_->yHeight = font.size;
		}

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
	*pstyle = TXTBACK_TRANSPARENT;
	return S_OK;
}


HRESULT TextBox::TextHost::TxGetMaxLength(DWORD *plength) {

	auto text_box = text_box_.lock();
	if (text_box != nullptr) {
		*plength = text_box->GetMaximumLength();
	}
	else {
		*plength = kDefaultMaximumLength;
	}

	return S_OK;
}


HRESULT TextBox::TextHost::TxGetScrollBars(DWORD *pdwScrollBar) {
	*pdwScrollBar = ES_AUTOVSCROLL | ES_AUTOHSCROLL;
	return S_OK;
}


HRESULT TextBox::TextHost::TxGetPasswordChar(_Out_ TCHAR *pch) {

	auto text_box = text_box_.lock();
	if (text_box != nullptr) {
		*pch = text_box->GetPasswordCharacter();
	}
	else {
		*pch = kDefaultPasswordCharacter;
	}

	return S_OK;
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

	switch (iNotify) {

		case EN_CHANGE: {
			auto text_box = text_box_.lock();
			if (text_box != nullptr) {

				auto event = text_box->GetPropertyMap().TryGetProperty<TextBox::TextChangeEvent>(kTextChangeEventPropertyName);
				if (event != nullptr) {
					event->Trigger(text_box);
				}
			}
			break;
		}
	}

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
	property_bits(TXTBIT_ALLOWBEEP),
	max_length(kDefaultMaximumLength),
	password_char(kDefaultPasswordCharacter) {

}

}