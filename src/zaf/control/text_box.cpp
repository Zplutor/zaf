#include <zaf/control/text_box.h>
#include <cassert>
#include <zaf/base/assert.h>
#include <zaf/base/event_utility.h>
#include <zaf/base/log.h>
#include <zaf/graphic/alignment.h>
#include <zaf/graphic/canvas.h>
#include <zaf/graphic/renderer/renderer.h>
#include <zaf/graphic/font/font.h>
#include <zaf/parsing/parsers/text_box_parser.h>
#include <zaf/reflection/reflection_type_definition.h>
#include <zaf/window/caret.h>
#include <zaf/window/message/keyboard_message.h>
#include <zaf/window/message/message.h>
#include <zaf/window/message/mouse_message.h>
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

static const wchar_t* const kAcceptReturnPropertyName = L"AcceptReturn";
static const wchar_t* const kAcceptTabPropertyName = L"AcceptTab";
static const wchar_t* const kInsetPropertyName = L"Inset";
static const wchar_t* const kMaximumLengthPropertyName = L"MaximumLength";
static const wchar_t* const kPasswordCharacterPropertyName = L"PasswordCharacter";
static const wchar_t* const kScrollBarChangeEventPropertyName = L"ScrollBarChangeEvent";
static const wchar_t* const kScrollValuesChangeEventPropertyName = L"ScrollValuesChangeEvent";
static const wchar_t* const kSelectionChangeEventPropertyName = L"SelectionChangeEvent";
static const wchar_t* const kTextValidatorPropertyName = L"TextValidator";

static const DWORD kDefaultPropertyBits = TXTBIT_ALLOWBEEP;
static const Frame kDefaultInset;
static const std::uint32_t kDefaultMaximumLength = std::numeric_limits<std::uint32_t>::max();
static const wchar_t kDefaultPasswordCharacter = L'*';
static const DWORD kDefaultScrollBarProperty = ES_AUTOVSCROLL | ES_AUTOHSCROLL | WS_VSCROLL | WS_HSCROLL;


ZAF_DEFINE_REFLECTION_TYPE(TextBox)
	ZAF_DEFINE_PARSER(TextBoxParser)
ZAF_DEFINE_END


TextBox::TextBox() : 
	property_bits_(kDefaultPropertyBits),
	character_format_(),
	paragraph_format_(),
    scroll_bar_property_(kDefaultScrollBarProperty),
    required_height_(0),
    text_color_(Color::Black) {

	character_format_.cbSize = sizeof(character_format_);
	paragraph_format_.cbSize = sizeof(paragraph_format_);
}


TextBox::~TextBox() {

}


void TextBox::Initialize() {

	__super::Initialize();

	SetCanFocused(true);
	SetBorder(1);

    //Initialize CHARFORMATW and PARAFORMAT.
	SetFont(Font::GetDefault());
	SetTextAlignment(TextAlignment::Leading);
    SetParagraphAlignment(ParagraphAlignment::Near);

    SetBorderColorPicker([](const Control&) {
        return Color::Black;
    });

    SetBackgroundColorPicker([](const Control& control) {

        const auto& text_box = dynamic_cast<const TextBox&>(control);

        if (text_box.IsReadOnly() || !text_box.IsEnabled()) {
            return Color::FromRGB(0xEEEEEE);;
        }

        return Color::White;
    });

    SetTextColorPicker([](const Control& control) {
        
        if (control.IsEnabled()) {
            return Color::Black;
        }
        else {
            return Color::Gray;
        }
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

    text_service_->TxSendMessage(
        EM_SETEVENTMASK, 0, ENM_CHANGE | ENM_SELCHANGE | ENM_PROTECTED,
        nullptr);
}


void TextBox::Layout(const Rect& previous_rect) {

	if (text_service_ != nullptr) {
		text_service_->OnTxPropertyBitsChange(TXTBIT_CLIENTRECTCHANGE, TXTBIT_CLIENTRECTCHANGE);
	}
}


void TextBox::Paint(Canvas& canvas, const Rect& dirty_rect) {

	__super::Paint(canvas, dirty_rect);

	if (text_service_ == nullptr) {
		return;
	}

    //If text color got from text color picker has been changed, set it to CHARFORMAT 
    //before painting.
    ReviseTextColor();

	auto render_target = canvas.GetRenderer().GetHandle();
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

    auto absolute_content_rect = GetAbsoluteContentRect();
    absolute_content_rect.position.y += GetPaintContentOffset(hdc);
    absolute_content_rect = Align(absolute_content_rect);

    RECT win32_absolute_rect = absolute_content_rect.ToRECT();
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
		0);

	gdi_interop_render_target->ReleaseDC(nullptr);
}


void TextBox::ReviseTextColor() {

    auto text_color = GetTextColor();
    if (text_color_ != text_color) {

        text_color_ = text_color;

        character_format_.dwMask |= CFM_COLOR;
        character_format_.crTextColor = text_color_.ToCOLORREF();

        if (text_service_ != nullptr) {
            text_service_->OnTxPropertyBitsChange(TXTBIT_CHARFORMATCHANGE, TXTBIT_CHARFORMATCHANGE);
        }
    }
}


float TextBox::GetPaintContentOffset(HDC hdc) {

    if (IsMultiline()) {
        return 0;
    }
    
    auto paragraph_alignment = GetParagraphAlignment();
    if (paragraph_alignment == ParagraphAlignment::Near) {
        return 0;
    }

    if (required_height_ == 0) {
        if (hdc == nullptr) {
            return 0;
        }

        LONG width = std::numeric_limits<LONG>::max();
        LONG height = 0;
        SIZEL extent_size = { -1, -1 };

        HRESULT result = text_service_->TxGetNaturalSize(
            DVASPECT_CONTENT,
            hdc,
            nullptr,
            nullptr,
            TXTNS_FITTOCONTENT,
            &extent_size,
            &width,
            &height);

        if (SUCCEEDED(result)) {
            required_height_ = static_cast<float>(height);
        }
    }

    if (paragraph_alignment == ParagraphAlignment::Center) {
        return (GetContentSize().height - required_height_) / 2;
    }
    else if (paragraph_alignment == ParagraphAlignment::Far) {
        return GetContentSize().height - required_height_;
    }
    else {
        return 0;
    }
}


void TextBox::ResetRequiredHeight() {
    required_height_ =0 ;
}


Rect TextBox::GetTextRect() {
    return Rect();
}


const Rect TextBox::GetAbsoluteContentRect() const {

	auto rect = GetAbsoluteRect();
	rect.Deflate(GetBorder());
    rect.Deflate(GetPadding());
	return rect;
}


Frame TextBox::GetInset() const {

    auto inset = GetPropertyMap().TryGetProperty<Frame>(kInsetPropertyName);
    if (inset != nullptr) {
        return *inset;
    }
    else {
        return kDefaultInset;
    }
}


void TextBox::SetInset(const Frame& inset) {

    GetPropertyMap().SetProperty(kInsetPropertyName, inset);

    if (text_service_ != nullptr) {
        text_service_->OnTxPropertyBitsChange(TXTBIT_VIEWINSETCHANGE, TXTBIT_VIEWINSETCHANGE);
    }
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


TextRange TextBox::GetSelectionRange() const {

	TextRange range;

	if (text_service_ != nullptr) {

		CHARRANGE char_range = { 0 };
		text_service_->TxSendMessage(EM_EXGETSEL, 0, reinterpret_cast<LPARAM>(&char_range), nullptr);

		range.index = char_range.cpMin < 0 ? 0 : char_range.cpMin;
		auto length = char_range.cpMax - char_range.cpMin;
		range.length = length < 0 ? std::numeric_limits<std::size_t>::max() : length;
	}

	return range;
}

void TextBox::SetSelectionRange(const TextRange& range) {

	if (text_service_ == nullptr) {
		return;
	}

	CHARRANGE char_range = { 0 };
	char_range.cpMin = range.index;
	char_range.cpMax = range.index + range.length;
	text_service_->TxSendMessage(EM_EXSETSEL, 0, reinterpret_cast<LPARAM>(&char_range), nullptr);
}


std::wstring TextBox::GetText() const {

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

        //Sometimes the text service would not require to repaint after
        //setting text, so do it here.
        NeedRepaint();
	}
}


TextValidator TextBox::GetTextValidator() const {

    auto validator = GetPropertyMap().TryGetProperty<TextValidator>(kTextValidatorPropertyName);
    if (validator != nullptr) {
        return *validator;
    }

    return nullptr;
}

void TextBox::SetTextValidator(const TextValidator& validator) {

    GetPropertyMap().SetProperty(kTextValidatorPropertyName, validator);
}


Font TextBox::GetFont() const {

	Font font;
	font.family_name = character_format_.szFaceName;
	font.size = static_cast<float>(character_format_.yHeight) / 15;
    font.weight = (character_format_.dwEffects & CFE_BOLD) ? FontWeight::Bold : FontWeight::Regular;
	return font;
}

void TextBox::SetFont(const Font& font) {

    ResetRequiredHeight();

    character_format_.dwMask |= CFM_PROTECTED;
    character_format_.dwEffects |= CFM_PROTECTED;

	character_format_.dwMask |= CFM_FACE;
	wcscpy_s(character_format_.szFaceName, font.family_name.c_str());

	character_format_.dwMask |= CFM_SIZE;
	character_format_.yHeight = static_cast<LONG>(font.size * 15);

    character_format_.dwMask |= CFM_BOLD;
    if (font.weight > FontWeight::Bold) {
        character_format_.dwEffects |= CFE_BOLD;
    }
    else {
        character_format_.dwEffects &= ~CFE_BOLD;
    }

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


void TextBox::SetAllowVerticalScroll(bool allow) {
    ChangeScrollBarPropertyBits(ES_AUTOVSCROLL | WS_VSCROLL, allow);
}


void TextBox::SetAllowHorizontalScroll(bool allow) {
    ChangeScrollBarPropertyBits(ES_AUTOHSCROLL | WS_HSCROLL, allow);
}


void TextBox::SetAutoHideScrollBars(bool auto_hide) {
    ChangeScrollBarPropertyBits(ES_DISABLENOSCROLL, ! auto_hide);
}


void TextBox::ChangeScrollBarPropertyBits(DWORD bits, bool is_set) {

    if (is_set) {
        scroll_bar_property_ |= bits;
    }
    else {
        scroll_bar_property_ &= ~bits;
    }

    if (text_service_ != nullptr) {
        text_service_->OnTxPropertyBitsChange(TXTBIT_SCROLLBARCHANGE, TXTBIT_SCROLLBARCHANGE);
    }
}


bool TextBox::CanShowVerticalScrollBar() {
    return CanEnableVerticalScrollBar() || (scroll_bar_property_ & ES_DISABLENOSCROLL);
}


bool TextBox::CanShowHorizontalScrollBar() {
    return CanEnableHorizontalScrollBar() || (scroll_bar_property_ & ES_DISABLENOSCROLL);
}


bool TextBox::CanEnableVerticalScrollBar() {

    BOOL is_enabled = FALSE;

    if (text_service_ != nullptr) {
        text_service_->TxGetVScroll(nullptr, nullptr, nullptr, nullptr, &is_enabled);
    }

    return (is_enabled != FALSE);
}


bool TextBox::CanEnableHorizontalScrollBar() {

    BOOL is_enabled = FALSE;

    if (text_service_ != nullptr) {
        text_service_->TxGetHScroll(nullptr, nullptr, nullptr, nullptr, &is_enabled);
    }

    return (is_enabled != FALSE);
}


void TextBox::GetVerticalScrollValues(int& current_value, int& min_value, int& max_value) {
    GetScrollValues(false, current_value, min_value, max_value);
}


void TextBox::GetHorizontalScrollValues(int& current_value, int& min_value, int& max_value) {
    GetScrollValues(true, current_value, min_value, max_value);
}


void TextBox::GetScrollValues(bool is_horizontal, int& current_value, int& min_value, int& max_value) {

    if (text_service_ == nullptr) {
        return;
    }

    LONG current = 0;
    LONG min = 0;
    LONG max = 0;
    LONG page = 0;
    HRESULT result = 0;
    
    if (is_horizontal) {
        result = text_service_->TxGetHScroll(&min, &max, &current, &page, nullptr);
    }
    else {
        result = text_service_->TxGetVScroll(&min, &max, &current, &page, nullptr);
    }

    if (SUCCEEDED(result)) {
        current_value = current;
        min_value = min;
        if (max < page) {
            max_value = 0;
        }
        else {
            max_value = max - page;
        }
    }
}


TextBox::SelectionChangeEvent::Proxy TextBox::GetSelectionChangeEvent() {
    return GetEventProxyFromPropertyMap<SelectionChangeEvent>(GetPropertyMap(), kSelectionChangeEventPropertyName);
}


TextBox::ScrollBarChangeEvent::Proxy TextBox::GetScrollBarChangeEvent() {
    return GetEventProxyFromPropertyMap<ScrollBarChangeEvent>(GetPropertyMap(), kScrollBarChangeEventPropertyName);
}


TextBox::ScrollValuesChangeEvent::Proxy TextBox::GetScrollValuesChangeEvent() {
    return GetEventProxyFromPropertyMap<ScrollValuesChangeEvent>(GetPropertyMap(), kScrollValuesChangeEventPropertyName);
}


bool TextBox::AcceptKeyMessage(const KeyMessage& message) const {

    switch (message.GetVirtualKey()) {
    case VK_TAB:
        return AcceptTab();

    case VK_RETURN:
        return AcceptReturn();

    default:
        return __super::AcceptKeyMessage(message);
    }
}


bool TextBox::AcceptTab() const {

    auto accept_tab = GetPropertyMap().TryGetProperty<bool>(kAcceptTabPropertyName);
    if (accept_tab != nullptr) {
        return *accept_tab;
    }
    else {
        return false;
    }
}

void TextBox::SetAcceptTab(bool accept_tab) {
    GetPropertyMap().SetProperty(kAcceptTabPropertyName, accept_tab);
}


bool TextBox::AcceptReturn() const {

    auto accept_return = GetPropertyMap().TryGetProperty<bool>(kAcceptReturnPropertyName);
    if (accept_return != nullptr) {
        return *accept_return;
    }
    else {
        return false;
    }
}

void TextBox::SetAcceptReturn(bool accept_return) {
    GetPropertyMap().SetProperty(kAcceptReturnPropertyName, accept_return);
}


void TextBox::ChangeMouseCursor(const Message& message, bool& is_changed) {

	if (ChangeMouseCursor()) {
		is_changed = true;
	}
	else {
		__super::ChangeMouseCursor(message, is_changed);
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


bool TextBox::MouseMove(const Point& position, const MouseMessage& message) {
	if (text_service_ != nullptr) {
		text_service_->TxSendMessage(WM_MOUSEMOVE, message.wparam, message.lparam, nullptr);
	}
    return true;
}


bool TextBox::MouseDown(const Point& position, const MouseMessage& message) {
	SetIsFocused(true);
	if (text_service_ != nullptr) {
		text_service_->TxSendMessage(WM_LBUTTONDOWN, message.wparam, message.lparam, nullptr);
	}
    return true;
}


bool TextBox::MouseUp(const Point& position, const MouseMessage& message) {
	if (text_service_ != nullptr) {
		text_service_->TxSendMessage(WM_LBUTTONUP, message.wparam, message.lparam, nullptr);
	}
    return true;
}


bool TextBox::KeyDown(const KeyMessage& message) {
    
	if (text_service_ != nullptr) {
		HRESULT result = text_service_->TxSendMessage(WM_KEYDOWN, message.wparam, message.lparam, nullptr);
        if (result == S_OK) {
            return true;
        }
	}
    
    return __super::KeyDown(message);
}


bool TextBox::KeyUp(const KeyMessage& message) {

	if (text_service_ != nullptr) {
		HRESULT result = text_service_->TxSendMessage(WM_KEYUP, message.wparam, message.lparam, nullptr);
        if (result == S_OK) {
            return true;
        }
	}
    
    return __super::KeyUp(message);
}


bool TextBox::CharInput(const CharMessage& message) {

	if (text_service_ != nullptr) {
		HRESULT result = text_service_->TxSendMessage(WM_CHAR, message.wparam, message.lparam, nullptr);
        if (result == S_OK) {
            return true;
        }
	}

    return __super::CharInput(message);
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


bool TextBox::CanUndo() const {

    if (text_service_ != nullptr) {
        LRESULT can_undo = FALSE;
        text_service_->TxSendMessage(EM_CANUNDO, 0, 0, &can_undo);
        return can_undo != FALSE;
    }
    else {
        return false;
    }
}


bool TextBox::Undo() {

    if (text_service_ != nullptr) {
        LRESULT undo_result = FALSE;
        text_service_->TxSendMessage(EM_UNDO, 0, 0, &undo_result);
        return undo_result != FALSE;
    }
    else {
        return false;
    }
}


bool TextBox::CanRedo() const {

    if (text_service_ != nullptr) {
        LRESULT can_redo = FALSE;
        text_service_->TxSendMessage(EM_CANREDO, 0, 0, &can_redo);
        return can_redo != FALSE;
    }
    else {
        return false;
    }
}


bool TextBox::Redo() {

    if (text_service_ != nullptr) {
        LRESULT redo_result = FALSE;
        text_service_->TxSendMessage(EM_REDO, 0, 0, &redo_result);
        return redo_result != FALSE;
    }
    else {
        return false;
    }
}


void TextBox::ScrollUpByLine() {
    SendScrollMessage(false, SB_LINEUP);
}


void TextBox::ScrollDownByLine() {
    SendScrollMessage(false, SB_LINEDOWN);
}


void TextBox::ScrollUpByPage() {
    SendScrollMessage(false, SB_PAGEUP);
}


void TextBox::ScrollDownByPage() {
    SendScrollMessage(false, SB_PAGEDOWN);
}


void TextBox::ScrollUpToBegin() {
    SendScrollMessage(false, SB_TOP);
}


void TextBox::ScrollDownToEnd() {
    SendScrollMessage(false, SB_BOTTOM);
}


void TextBox::ScrollLeftToBegin() {
    SendScrollMessage(true, SB_LEFT);
}


void TextBox::ScrollRightToEnd() {

    //Don't use WM_HSCROLL with SB_RIGHT, because it would scroll "too far",
    //not the result expected.
    int current_value = 0;
    int min_value = 0;
    int max_value = 0;
    GetScrollValues(true, current_value, min_value, max_value);
    HorizontallyScroll(max_value);

    //Sometimes the text service would not require to repaint after scrolling,
    //so do it here.
    NeedRepaint();
}


void TextBox::SendScrollMessage(bool is_horizontal, WORD scroll_type) {

    if (text_service_ != nullptr) {
        text_service_->TxSendMessage(is_horizontal ? WM_HSCROLL : WM_VSCROLL, MAKEWPARAM(scroll_type, 0), 0, nullptr);
    }
}


void TextBox::VerticallyScroll(int new_value) {
    Scroll(false, new_value);
}


void TextBox::HorizontallyScroll(int new_value) {
    Scroll(true, new_value);
}


void TextBox::Scroll(bool is_horizontal, int new_value) {

    if (text_service_ == nullptr) {
        return;
    }

    UINT message = is_horizontal ? WM_HSCROLL : WM_VSCROLL;
    text_service_->TxSendMessage(message, MAKEWPARAM(SB_THUMBPOSITION, new_value), 0, nullptr);
}


void TextBox::ScrollBarChange() {

    auto event = TryGetEventFromPropertyMap<ScrollBarChangeEvent>(GetPropertyMap(), kScrollBarChangeEventPropertyName);
    if (event != nullptr) {
        event->Trigger(*this);
    }
}


void TextBox::ScrollValuesChange(bool is_horizontal) {

    auto event = TryGetEventFromPropertyMap<ScrollValuesChangeEvent>(GetPropertyMap(), kScrollValuesChangeEventPropertyName);
    if (event != nullptr) {
        event->Trigger(*this, is_horizontal);
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

    auto text_box = text_box_.lock();
    if (text_box != nullptr) {
        text_box->ScrollBarChange();
    }
	return TRUE;
}


BOOL TextBox::TextHostBridge::TxEnableScrollBar(INT fuSBFlags, INT fuArrowflags) {

    auto text_box = text_box_.lock();
    if (text_box != nullptr) {
        text_box->ScrollBarChange();
    }
    return TRUE;
}


BOOL TextBox::TextHostBridge::TxSetScrollRange(INT fnBar, LONG nMinPos, INT nMaxPos, BOOL fRedraw) {

    auto text_box = text_box_.lock();
    if (text_box != nullptr) {
        text_box->ScrollValuesChange(fnBar == SB_HORZ);
    }
	return TRUE;
}


BOOL TextBox::TextHostBridge::TxSetScrollPos(INT fnBar, INT nPos, BOOL fRedraw) {

    auto text_box = text_box_.lock();
    if (text_box != nullptr) {
        text_box->ScrollValuesChange(fnBar == SB_HORZ);
    }
    return TRUE;
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

    auto text_box = text_box_.lock();
    if (text_box == nullptr) {
        return FALSE;
    }

    //Sometimes TxShowCaret would be called even when the 
    //text box is unfocused. This would mess up the caret,
    //so only response to this method while focused.
    if (! text_box->IsFocused()) {
        return FALSE;
    }

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

    auto text_box = text_box_.lock();
    if (text_box == nullptr) {
        return FALSE;
    }

    auto window = text_box->GetWindow();
    if (window == nullptr) {
        return FALSE;
    }
	
	const auto& caret = window->GetCaret();
	caret->SetPosition(Point(static_cast<float>(x), static_cast<float>(y + text_box->GetPaintContentOffset(nullptr))));
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

    if (fCapture) {
        text_box->CaptureMouse();
    }
    else {
        text_box->ReleaseMouse();
    }
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
		rect = Align(text_box->GetAbsoluteContentRect());
	}
    
	*prc = rect.ToRECT();
	return S_OK;
}


HRESULT TextBox::TextHostBridge::TxGetViewInset(LPRECT prc) {
	
    auto text_box = text_box_.lock();
    if (text_box != nullptr) {
        *prc = text_box->GetInset().ToRECT();
    }
    else {
        *prc = kDefaultInset.ToRECT();
    }

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

    auto text_box = text_box_.lock();
    if (text_box != nullptr) {
        *pdwScrollBar = text_box->scroll_bar_property_;
    }
    else {
        *pdwScrollBar = kDefaultScrollBarProperty;
    }

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
            text_box->NotifyTextChange();
            return S_OK;
		}

		case EN_SELCHANGE: {
            auto event = TryGetEventFromPropertyMap<TextBox::SelectionChangeEvent>(
                text_box->GetPropertyMap(),
                kSelectionChangeEventPropertyName
            );
			if (event != nullptr) {
				event->Trigger(text_box);
			}
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


bool TextBox::TextHostBridge::NotifyProtected(const ENPROTECTED& info) const {

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
            if (! is_succeeded) {
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