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

namespace zaf {

static ITextServices* CreateTextService(ITextHost* text_host);

static const wchar_t* const kMaximumLengthPropertyName = L"MaximumLength";
static const wchar_t* const kPasswordCharacterPropertyName = L"PasswordCharacter";
static const wchar_t* const kSelectionChangeEventPropertyName = L"SelectionChangeEvent";
static const wchar_t* const kTextChangeEventPropertyName = L"TextChangeEvent";

static const DWORD kDefaultPropertyBits = TXTBIT_ALLOWBEEP;
static const std::uint32_t kDefaultMaximumLength = std::numeric_limits<std::uint32_t>::max();
static const wchar_t kDefaultPasswordCharacter = L'*';

TextBox::TextBox() : 
	property_bits_(kDefaultPropertyBits),
	character_format_(),
	paragraph_format_() {

	character_format_.cbSize = sizeof(character_format_);
	paragraph_format_.cbSize = sizeof(paragraph_format_);
}


TextBox::~TextBox() {

}


void TextBox::Initialize() {

	__super::Initialize();

	SetCanFocused(true);
	SetBorderWidth(1);
	SetFont(Font());
	SetTextAlignment(TextAlignment::Leading);

    SetBorderColorPicker([](const Control&) {
        return Color::Black;
    });

    SetBackgroundColorPicker([](const Control& control) {

        const auto& text_box = dynamic_cast<const TextBox&>(control);

        if (text_box.IsReadOnly()) {
            return Color::FromRGB(0xEEEEEE);;
        }

        return Color::White;
    });

	InitializeTextService();
}


void TextBox::InitializeTextService() {

	auto shared_this = std::dynamic_pointer_cast<TextBox>(shared_from_this());
	text_host_bridge_ = std::make_shared<TextHostBridge>(shared_this);

	text_service_ = CreateTextService(text_host_bridge_.get());
	if (text_service_ == nullptr) {
		return;
	}

	text_service_->OnTxInPlaceActivate(nullptr);
	text_service_->TxSendMessage(EM_SETEVENTMASK, 0, ENM_CHANGE | ENM_SELCHANGE, nullptr);
}


void TextBox::Layout(const Rect& previous_rect) {

	if (text_service_ != nullptr) {
		text_service_->OnTxPropertyBitsChange(TXTBIT_CLIENTRECTCHANGE, TXTBIT_CLIENTRECTCHANGE);
	}
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


const Rect TextBox::GetTextRect() const {
    return Rect();
}


const Rect TextBox::GetAbsoluteContentRect() const {

	auto rect = GetAbsoluteRect();
	rect.Inflate(-GetBorderWidth());
	return rect;
}


std::uint32_t TextBox::GetMaximumLength() const {

	auto max_length = GetPropertyMap().TryGetProperty<std::uint32_t>(kMaximumLengthPropertyName);
	if (max_length != nullptr) {
		return *max_length;
	}
	else {
		return kDefaultMaximumLength;
	}
}


void TextBox::SetMaximumLength(std::uint32_t max_length) {

	GetPropertyMap().SetProperty(kMaximumLengthPropertyName, max_length);

	if (text_service_ != nullptr) {
		text_service_->OnTxPropertyBitsChange(TXTBIT_MAXLENGTHCHANGE, TXTBIT_MAXLENGTHCHANGE);
	}
}


bool TextBox::UsePasswordCharacter() const {
	return HasPropertyBit(TXTBIT_USEPASSWORD);
}

void TextBox::SetUsePasswordCharacter(bool use_password_char) {
	ChangePropertyBit(TXTBIT_USEPASSWORD, use_password_char);
}


wchar_t TextBox::GetPasswordCharacter() const {
	
	auto password_char = GetPropertyMap().TryGetProperty<wchar_t>(kPasswordCharacterPropertyName);
	if (password_char != nullptr) {
		return *password_char;
	}
	else {
		return kDefaultPasswordCharacter;
	}
}

void TextBox::SetPasswordCharacter(wchar_t password_char) {

	GetPropertyMap().SetProperty(kPasswordCharacterPropertyName, password_char);

	if ((text_service_ != nullptr) && UsePasswordCharacter()) {
		text_service_->OnTxPropertyBitsChange(TXTBIT_USEPASSWORD, TXTBIT_USEPASSWORD);
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
    NeedRepaint();
}


bool TextBox::AllowBeep() const {
	return HasPropertyBit(TXTBIT_ALLOWBEEP);
}

void TextBox::SetAllowBeep(bool allow_beep) {
	ChangePropertyBit(TXTBIT_ALLOWBEEP, allow_beep);
}


const Range TextBox::GetSelectionRange() const {

	Range range;

	if (text_service_ != nullptr) {

		CHARRANGE char_range = { 0 };
		text_service_->TxSendMessage(EM_EXGETSEL, 0, reinterpret_cast<LPARAM>(&char_range), nullptr);

		range.position = char_range.cpMin < 0 ? 0 : char_range.cpMin;
		auto length = char_range.cpMax - char_range.cpMin;
		range.length = length < 0 ? std::numeric_limits<std::size_t>::max() : length;
	}

	return range;
}

void TextBox::SetSelectionRange(const Range& range) {

	if (text_service_ == nullptr) {
		return;
	}

	CHARRANGE char_range = { 0 };
	char_range.cpMin = range.position;
	char_range.cpMax = range.position + range.length;
	text_service_->TxSendMessage(EM_EXSETSEL, 0, reinterpret_cast<LPARAM>(&char_range), nullptr);
}


const std::wstring TextBox::GetText() const {

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

	if (text_service_ != nullptr) {
		text_service_->TxSetText(text.c_str());
	}
}


const Font TextBox::GetFont() const {

	Font font;
	font.family_name = character_format_.szFaceName;
	font.size = static_cast<float>(character_format_.yHeight);
	return font;
}

void TextBox::SetFont(const Font& font) {

	character_format_.dwMask |= CFM_FACE;
	wcscpy_s(character_format_.szFaceName, font.family_name.c_str());

	character_format_.dwMask |= CFM_SIZE;
	character_format_.yHeight = static_cast<LONG>(font.size);

	if (text_service_ != nullptr) {
		text_service_->OnTxPropertyBitsChange(TXTBIT_CHARFORMATCHANGE, TXTBIT_CHARFORMATCHANGE);
	}
}


TextAlignment TextBox::GetTextAlignment() const {

	switch (paragraph_format_.wAlignment) {
		case PFA_CENTER:
			return TextAlignment::Center;
		case PFA_LEFT:
			return TextAlignment::Leading;
		case PFA_RIGHT:
			return TextAlignment::Tailing;
		default:
			return TextAlignment::Leading;
	}
}

void TextBox::SetTextAlignment(TextAlignment alignment) {

	paragraph_format_.dwMask |= PFM_ALIGNMENT;

	switch (alignment) {
		case TextAlignment::Center:
			paragraph_format_.wAlignment = PFA_CENTER;
			break;
		case TextAlignment::Leading:
			paragraph_format_.wAlignment = PFA_LEFT;
			break;
		case TextAlignment::Tailing:
			paragraph_format_.wAlignment = PFA_RIGHT;
			break;
		default:
			ZAF_FAIL();
			break;
	}

	if (text_service_ != nullptr) {
		text_service_->OnTxPropertyBitsChange(TXTBIT_PARAFORMATCHANGE, TXTBIT_PARAFORMATCHANGE);
	}
}


WordWrapping TextBox::GetWordWrapping() const {
	return HasPropertyBit(TXTBIT_WORDWRAP) ? WordWrapping::Wrap : WordWrapping::NoWrap;
}

void TextBox::SetWordWrapping(WordWrapping word_wrapping) {
	ChangePropertyBit(TXTBIT_WORDWRAP, word_wrapping != WordWrapping::NoWrap);
}


bool TextBox::HasPropertyBit(DWORD bit) const {
	return (property_bits_ & bit) != 0;
}


void TextBox::ChangePropertyBit(DWORD bit, bool is_set) {

	if (is_set) {
		property_bits_ |= bit;
	}
	else {
		property_bits_ &= ~bit;
	}

	if (text_service_ != nullptr) {
		text_service_->OnTxPropertyBitsChange(bit, property_bits_);
	}
}


TextBox::TextChangeEvent::Proxy TextBox::GetTextChangeEvent() {

	auto& event = GetPropertyMap().GetProperty<TextChangeEvent>(kTextChangeEventPropertyName);
	return TextChangeEvent::Proxy(event);
}


TextBox::SelectionChangeEvent::Proxy TextBox::GetSelectionChangeEvent() {

	auto& event = GetPropertyMap().GetProperty<SelectionChangeEvent>(kSelectionChangeEventPropertyName);
	return SelectionChangeEvent::Proxy(event);
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
		static_cast<INT>(mouse_position.x),
		static_cast<INT>(mouse_position.y)
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


TextBox::TextHostBridge::TextHostBridge(const std::shared_ptr<TextBox>& text_box) :
	text_box_(text_box) {

}


HRESULT TextBox::TextHostBridge::QueryInterface(REFIID riid, void** ppvObject) {

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


HDC TextBox::TextHostBridge::TxGetDC() {
	return GetDC(GetWindowHandle());
}


INT TextBox::TextHostBridge::TxReleaseDC(HDC hdc) {
	return ReleaseDC(GetWindowHandle(), hdc);
}


BOOL TextBox::TextHostBridge::TxShowScrollBar(INT fnBar, BOOL fShow) {
	return FALSE;
}


BOOL TextBox::TextHostBridge::TxEnableScrollBar(INT fuSBFlags, INT fuArrowflags) {
	return FALSE;
}


BOOL TextBox::TextHostBridge::TxSetScrollRange(INT fnBar, LONG nMinPos, INT nMaxPos, BOOL fRedraw) {
	return FALSE;
}


BOOL TextBox::TextHostBridge::TxSetScrollPos(INT fnBar, INT nPos, BOOL fRedraw) {
	return FALSE;
}


void TextBox::TextHostBridge::TxInvalidateRect(LPCRECT prc, BOOL fMode) {
	auto text_box = text_box_.lock();
	if (text_box != nullptr) {
		text_box->NeedRepaint();
	}
}


void TextBox::TextHostBridge::TxViewChange(BOOL fUpdate) {
}


BOOL TextBox::TextHostBridge::TxCreateCaret(HBITMAP hbmp, INT xWidth, INT yHeight) {

	auto window = GetWindow();
	if (window == nullptr) {
		return FALSE;
	}

	window->GetCaret()->SetSize(Size(static_cast<float>(xWidth), static_cast<float>(yHeight)));
	return TRUE;
}


BOOL TextBox::TextHostBridge::TxShowCaret(BOOL fShow) {

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


BOOL TextBox::TextHostBridge::TxSetCaretPos(INT x, INT y) {

	auto window = GetWindow();
	if (window == nullptr) {
		return FALSE;
	}

	const auto& caret = window->GetCaret();
	caret->SetPosition(Point(static_cast<float>(x), static_cast<float>(y)));
	return TRUE;
}


BOOL TextBox::TextHostBridge::TxSetTimer(UINT idTimer, UINT uTimeout) {
	return FALSE;
}


void TextBox::TextHostBridge::TxKillTimer(UINT idTimer) {
}


void TextBox::TextHostBridge::TxScrollWindowEx(INT dx, INT dy, LPCRECT lprcScroll, LPCRECT lprcClip, HRGN hrgnUpdate, LPRECT lprcUpdate, UINT fuScroll) {
}


void TextBox::TextHostBridge::TxSetCapture(BOOL fCapture) {

	auto text_box = text_box_.lock();
	if (text_box == nullptr) {
		return;
	}

	text_box->NeedCaptureMouse(fCapture == TRUE);
}


void TextBox::TextHostBridge::TxSetFocus() {
	
}


void TextBox::TextHostBridge::TxSetCursor(HCURSOR hcur, BOOL fText) {
	SetCursor(hcur);
}


BOOL TextBox::TextHostBridge::TxScreenToClient(LPPOINT lppt) {
	return FALSE;
}


BOOL TextBox::TextHostBridge::TxClientToScreen(LPPOINT lppt) {
	return FALSE;
}


HRESULT TextBox::TextHostBridge::TxActivate(LONG * plOldState) {
	return E_NOTIMPL;
}


HRESULT TextBox::TextHostBridge::TxDeactivate(LONG lNewState) {
	return E_NOTIMPL;
}


HRESULT TextBox::TextHostBridge::TxGetClientRect(LPRECT prc) {

	Rect rect;

	auto text_box = text_box_.lock();
	if (text_box != nullptr) {
		rect = text_box->GetAbsoluteContentRect();
	}

	*prc = rect.ToRECT();
	return S_OK;
}


HRESULT TextBox::TextHostBridge::TxGetViewInset(LPRECT prc) {
	*prc = RECT();
	return S_OK;
}


HRESULT TextBox::TextHostBridge::TxGetCharFormat(const CHARFORMATW **ppCF) {

	auto text_box = text_box_.lock(); 
	if (text_box != nullptr) {
		*ppCF = &(text_box->character_format_);
		return S_OK;
	}
	else {
		return E_NOTIMPL;
	}
}


HRESULT TextBox::TextHostBridge::TxGetParaFormat(const PARAFORMAT **ppPF) {

	auto text_box = text_box_.lock();
	if (text_box != nullptr) {
		*ppPF = &(text_box->paragraph_format_);
		return S_OK;
	}
	else {
		return E_NOTIMPL;
	}
}


COLORREF TextBox::TextHostBridge::TxGetSysColor(int nIndex) {
	return GetSysColor(nIndex);
}


HRESULT TextBox::TextHostBridge::TxGetBackStyle(TXTBACKSTYLE *pstyle) {
	*pstyle = TXTBACK_TRANSPARENT;
	return S_OK;
}


HRESULT TextBox::TextHostBridge::TxGetMaxLength(DWORD *plength) {

	auto text_box = text_box_.lock();
	if (text_box != nullptr) {
		*plength = text_box->GetMaximumLength();
	}
	else {
		*plength = kDefaultMaximumLength;
	}

	return S_OK;
}


HRESULT TextBox::TextHostBridge::TxGetScrollBars(DWORD *pdwScrollBar) {
	*pdwScrollBar = ES_AUTOVSCROLL | ES_AUTOHSCROLL;
	return S_OK;
}


HRESULT TextBox::TextHostBridge::TxGetPasswordChar(_Out_ TCHAR *pch) {

	auto text_box = text_box_.lock();
	if (text_box != nullptr) {
		*pch = text_box->GetPasswordCharacter();
	}
	else {
		*pch = kDefaultPasswordCharacter;
	}

	return S_OK;
}


HRESULT TextBox::TextHostBridge::TxGetAcceleratorPos(LONG *pcp) {
	return E_NOTIMPL;
}


HRESULT TextBox::TextHostBridge::TxGetExtent(LPSIZEL lpExtent) {
	return E_NOTIMPL;
}


HRESULT TextBox::TextHostBridge::OnTxCharFormatChange(const CHARFORMATW * pCF) {
	return E_NOTIMPL;
}


HRESULT TextBox::TextHostBridge::OnTxParaFormatChange(const PARAFORMAT * pPF) {
	return E_NOTIMPL;
}


HRESULT TextBox::TextHostBridge::TxGetPropertyBits(DWORD dwMask, DWORD *pdwBits) {

	auto text_box = text_box_.lock();
	if (text_box != nullptr) {
		*pdwBits = dwMask & text_box->property_bits_;
	}
	else {
		*pdwBits = kDefaultPropertyBits;
	}
	
	return S_OK;
}


HRESULT TextBox::TextHostBridge::TxNotify(DWORD iNotify, void *pv) {

	auto text_box = text_box_.lock();
	if (text_box == nullptr) {
		return S_OK;
	}

	switch (iNotify) {

		case EN_CHANGE: {
			auto event = text_box->GetPropertyMap().TryGetProperty<TextBox::TextChangeEvent>(kTextChangeEventPropertyName);
			if (event != nullptr) {
				event->Trigger(text_box);
			}
			break;
		}

		case EN_SELCHANGE: {
			auto event = text_box->GetPropertyMap().TryGetProperty<TextBox::SelectionChangeEvent>(kSelectionChangeEventPropertyName);
			if (event != nullptr) {
				event->Trigger(text_box);
			}
			break;
		}
	}

	return S_OK;
}


HIMC TextBox::TextHostBridge::TxImmGetContext() {
	return nullptr;
}


void TextBox::TextHostBridge::TxImmReleaseContext(HIMC himc) {
}


HRESULT TextBox::TextHostBridge::TxGetSelectionBarWidth(LONG *lSelBarWidth) {
	*lSelBarWidth = 0;
	return S_OK;
}


std::shared_ptr<Window> TextBox::TextHostBridge::GetWindow() const {

	auto text_box = text_box_.lock();
	if (text_box == nullptr) {
		return nullptr;
	}

	return text_box->GetWindow();
}


HWND TextBox::TextHostBridge::GetWindowHandle() const {

	auto window = GetWindow();
	if (window == nullptr) {
		return nullptr;
	}

	return window->GetHandle();
}


static ITextServices* CreateTextService(ITextHost* text_host) {

	HMODULE module_handle = LoadLibrary(L"riched20.dll");
	if (module_handle == nullptr) {
		return nullptr;
	}

	typedef HRESULT(_stdcall*CreateTextServicesFunction)(IUnknown*, ITextHost*, IUnknown**);
	auto create_text_services = reinterpret_cast<CreateTextServicesFunction>(
		GetProcAddress(module_handle, "CreateTextServices")
	);
	if (create_text_services == nullptr) {
		return nullptr;
	}

	CComPtr<IUnknown> service;
	HRESULT result = create_text_services(nullptr, text_host, &service);
	if (FAILED(result)) {
		return nullptr;
	}

	ITextServices* text_service = nullptr;
	service->QueryInterface(IID_ITextServices, reinterpret_cast<void**>(&text_service));
	return text_service;
}


}