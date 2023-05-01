#include <zaf/control/rich_edit.h>
#include <atlwin.h>
#include <tom.h>
#include <cassert>
#include <zaf/base/error/system_error.h>
#include <zaf/base/event_utility.h>
#include <zaf/base/log.h>
#include <zaf/control/caret.h>
#include <zaf/control/rich_edit/embedded_object.h>
#include <zaf/control/rich_edit/internal/ole_helper.h>
#include <zaf/control/rich_edit/internal/text_host_bridge.h>
#include <zaf/graphic/alignment.h>
#include <zaf/graphic/canvas.h>
#include <zaf/graphic/dpi.h>
#include <zaf/graphic/renderer/renderer.h>
#include <zaf/graphic/font/font.h>
#include <zaf/object/type_definition.h>
#include <zaf/rx/scheduler.h>
#include <zaf/rx/timer.h>
#include <zaf/window/message/keyboard_message.h>
#include <zaf/window/message/message.h>
#include <zaf/window/message/mouse_message.h>
#include <zaf/window/window.h>

#undef max

EXTERN_C const IID IID_ITextHost = {
    0xc5bdd8d0,
    0xd26e,
    0x11ce,
    { 0xa8, 0x9e, 0x00, 0xaa, 0x00, 0x6c, 0xad, 0xc5 }
};

EXTERN_C const IID IID_ITextDocument = {
    0x8cc497c0,
    0xa1df,
    0x11ce,
    { 0x80, 0x98, 0x00, 0xaa, 0x00, 0x47, 0xbe, 0x5d }
};

namespace zaf {
namespace {

COMObject<ITextServices2> CreateTextService(ITextHost* text_host) {

    HMODULE module_handle = LoadLibrary(L"msftedit.dll");
    if (!module_handle) {
        ZAF_THROW_SYSTEM_ERROR(GetLastError());
    }

    using CreateTextServicesFunction = HRESULT(_stdcall*)(IUnknown*, ITextHost*, IUnknown**);
    auto create_function = reinterpret_cast<CreateTextServicesFunction>(
        GetProcAddress(module_handle, "CreateTextServices"));

    if (!create_function) {
        ZAF_THROW_SYSTEM_ERROR(GetLastError());
    }

    auto iid_text_service2 = reinterpret_cast<const IID*>(
        GetProcAddress(module_handle, "IID_ITextServices2"));

    if (!iid_text_service2) {
        ZAF_THROW_SYSTEM_ERROR(GetLastError());
    }

    COMObject<IUnknown> unknown;
    HRESULT hresult = create_function(nullptr, text_host, unknown.Store());
    ZAF_THROW_IF_COM_ERROR(hresult);

    COMObject<ITextServices2> text_service;
    hresult = unknown->QueryInterface(*iid_text_service2, text_service.StoreVoid());
    ZAF_THROW_IF_COM_ERROR(hresult);

    return text_service;
}

constexpr DWORD kDefaultPropertyBits = TXTBIT_D2DDWRITE | TXTBIT_ALLOWBEEP;
constexpr std::uint32_t kDefaultMaxLength = std::numeric_limits<std::uint32_t>::max();
constexpr wchar_t kDefaultPasswordCharacter = L'*';
constexpr DWORD kDefaultScrollBarProperty = 
    ES_AUTOVSCROLL | ES_AUTOHSCROLL | WS_VSCROLL | WS_HSCROLL;

}

static const wchar_t* const kAcceptReturnPropertyName = L"AcceptReturn";
static const wchar_t* const kAcceptTabPropertyName = L"AcceptTab";
static const wchar_t* const kInsetPropertyName = L"Inset";
static const wchar_t* const kMaxLengthPropertyName = L"MaxLength";
static const wchar_t* const kPasswordCharacterPropertyName = L"PasswordCharacter";
static const wchar_t* const kScrollBarChangeEventPropertyName = L"ScrollBarChangeEvent";
static const wchar_t* const kScrollValuesChangeEventPropertyName = L"ScrollValuesChangeEvent";
static const wchar_t* const kSelectionChangedEventPropertyName = L"SelectionChangedEvent";
static const wchar_t* const kTextValidatorPropertyName = L"TextValidator";


ZAF_DEFINE_TYPE(RichEdit)
ZAF_DEFINE_TYPE_PROPERTY(IsReadOnly)
ZAF_DEFINE_TYPE_PROPERTY(IsMultiline)
ZAF_DEFINE_TYPE_END


RichEdit::RichEdit() : 
    property_bits_(kDefaultPropertyBits),
    character_format_(),
    paragraph_format_(),
    scroll_bar_property_(kDefaultScrollBarProperty),
    text_color_(Color::Black()) {

    character_format_.cbSize = sizeof(character_format_);
    paragraph_format_.cbSize = sizeof(paragraph_format_);
}


RichEdit::~RichEdit() {

}


void RichEdit::Initialize() {

    __super::Initialize();

    SetCanFocused(true);
    SetBorder(Frame(1));

    //Initialize CHARFORMATW and PARAFORMAT.
    SetFont(Font::Default());
    SetTextAlignment(TextAlignment::Leading);
    SetParagraphAlignment(ParagraphAlignment::Near);

    SetBorderColorPicker([](const Control&) {
        return Color::Black();
    });

    SetBackgroundColorPicker([](const Control& control) {

        const auto& rich_edit = dynamic_cast<const RichEdit&>(control);

        if (rich_edit.IsReadOnly() || !rich_edit.IsEnabled()) {
            return Color::FromRGB(0xEEEEEE);;
        }

        return Color::White();
    });

    SetTextColorPicker([](const Control& control) {
        
        if (control.IsEnabled()) {
            return Color::Black();
        }
        else {
            return Color::Gray();
        }
    });

    InitializeTextService();
}


void RichEdit::InitializeTextService() {

    auto shared_this = std::dynamic_pointer_cast<RichEdit>(shared_from_this());
    text_host_bridge_ = std::make_shared<rich_edit::internal::TextHostBridge>(shared_this);

    text_service_ = CreateTextService(text_host_bridge_.get());
    text_service_->OnTxInPlaceActivate(nullptr);
    text_service_->TxSendMessage(
        EM_SETEVENTMASK,
        0,
        ENM_CHANGE | ENM_SELCHANGE | ENM_PROTECTED, 
        nullptr);
}


void RichEdit::Layout(const zaf::Rect& previous_rect) {

    if (text_service_ != nullptr) {
        text_service_->OnTxPropertyBitsChange(TXTBIT_CLIENTRECTCHANGE, TXTBIT_CLIENTRECTCHANGE);
    }
}


void RichEdit::Paint(Canvas& canvas, const zaf::Rect& dirty_rect) {

    __super::Paint(canvas, dirty_rect);

    //If text color got from text color picker has been changed, set it to CHARFORMAT 
    //before painting.
    ReviseTextColor();

    //Note: all painting operations are in content coordinate.
    auto content_rect = this->ContentRect();
    auto content_region_guard = canvas.PushRegion(content_rect, dirty_rect);

    zaf::Rect bounds_in_content;
    bounds_in_content.position.y = GetContentVerticalOffset();
    bounds_in_content.size = content_rect.size;
    bounds_in_content = Align(bounds_in_content);
    auto bounds_rect = FromDIPs(bounds_in_content, this->GetDPI()).ToRECTL();

    zaf::Rect update_area_in_content = dirty_rect;
    update_area_in_content.SubtractOffset(content_rect.position);
    update_area_in_content = Align(update_area_in_content);
    auto update_rect = FromDIPs(update_area_in_content, this->GetDPI()).ToRECT();

    text_service_->TxDrawD2D(
        canvas.Renderer().Inner(),
        &bounds_rect,
        &update_rect,
        TXTVIEW_ACTIVE);

    PaintEmbeddedObjects(canvas, update_area_in_content);

    if (caret_) {
        caret_->Paint(canvas, update_area_in_content);
    }
}


void RichEdit::ReviseTextColor() {

    auto text_color = TextColor();
    if (text_color_ != text_color) {

        text_color_ = text_color;

        character_format_.dwMask |= CFM_COLOR;
        character_format_.crTextColor = text_color_.ToCOLORREF();

        if (text_service_ != nullptr) {
            text_service_->OnTxPropertyBitsChange(TXTBIT_CHARFORMATCHANGE, TXTBIT_CHARFORMATCHANGE);
        }
    }
}


void RichEdit::PaintEmbeddedObjects(Canvas& canvas, const zaf::Rect& dirty_rect) {

    auto ole_interface = GetOLEInterface();
    LONG object_count = ole_interface->GetObjectCount();
    if (object_count <= 0) {
        return;
    }

    auto text_document = ole_interface.Query<ITextDocument>();
    if (!text_document) {
        return;
    }

    auto absolute_content_rect = this->AbsoluteContentRect();
    auto selection_range = this->GetSelectionRange();

    for (LONG index = 0; index < object_count; ++index) {

        REOBJECT object_info{};
        object_info.cbStruct = sizeof(object_info);
        HRESULT hresult = ole_interface->GetObject(index, &object_info, REO_GETOBJ_NO_INTERFACES);
        if (FAILED(hresult)) {
            continue;
        }

        COMObject<ITextRange> text_range;
        hresult = text_document->Range(object_info.cp, object_info.cp, text_range.Store());
        if (FAILED(hresult)) {
            continue;
        }

        COMObject<IUnknown> unknown;
        hresult = text_range->GetEmbeddedObject(unknown.Store());
        if (FAILED(hresult)) {
            continue;
        }

        auto embedded_object = dynamic_cast<rich_edit::EmbeddedObject*>(unknown.Inner());
        if (!embedded_object) {
            continue;
        }

        //Note: GetPoint() returns position in window coordinate, 
        //see TextHostBridge::TxGetClientRect().
        long absolute_x{};
        long absolute_y{};
        hresult = text_range->GetPoint(
            tomAllowOffClient | tomClientCoord | TA_TOP | TA_LEFT,
            &absolute_x, 
            &absolute_y);

        if (FAILED(hresult)) {
            continue;
        }

        Point object_position{ static_cast<float>(absolute_x), static_cast<float>(absolute_y) };
        object_position = ToDIPs(object_position, this->GetDPI());
        object_position.SubtractOffset(absolute_content_rect.position);

        zaf::Rect object_rect{ object_position, embedded_object->Size() };
        auto dirty_rect_of_object = Rect::Intersect(dirty_rect, object_rect);
        if (dirty_rect_of_object.IsEmpty()) {
            continue;
        }

        auto region_guard = canvas.PushRegion(object_rect, dirty_rect_of_object);

        auto dirty_rect_in_object = dirty_rect_of_object;
        dirty_rect_in_object.SubtractOffset(object_rect.position);

        embedded_object->Paint(
            canvas,
            dirty_rect_in_object, 
            rich_edit::PaintContext{
                static_cast<std::size_t>(object_info.cp),
                selection_range.Contain(object_info.cp)
            }
        );
    }
}


float RichEdit::GetContentVerticalOffset() {

    //Due to some problems, multi-line rich edit doesn't support ParagraphAlignment for now.
    if (IsMultiline()) {
        return 0;
    }

    auto paragraph_alignment = ParagraphAlignment();
    if (paragraph_alignment == ParagraphAlignment::Near) {
        return 0;
    }

    if (!cached_text_height_) {

        LONG width = std::numeric_limits<LONG>::max();
        LONG height = 0;
        SIZEL extent_size = { -1, -1 };

        HRESULT result = text_service_->TxGetNaturalSize(
            DVASPECT_CONTENT,
            nullptr,
            nullptr,
            nullptr,
            TXTNS_FITTOCONTENT,
            &extent_size,
            &width,
            &height);

        if (FAILED(result)) {
            return 0;
        }

        cached_text_height_ = ToDIPs(static_cast<float>(height), this->GetDPI());
    }

    if (paragraph_alignment == ParagraphAlignment::Center) {
        return (ContentSize().height - *cached_text_height_) / 2;
    }
    else if (paragraph_alignment == ParagraphAlignment::Far) {
        return ContentSize().height - *cached_text_height_;
    }
    else {
        return 0;
    }
}


void RichEdit::ResetCachedTextHeight() {
    cached_text_height_.reset();
}


zaf::Rect RichEdit::GetTextRect() {
    return zaf::Rect();
}


Frame RichEdit::GetInset() const {

    auto inset = GetPropertyMap().TryGetProperty<Frame>(kInsetPropertyName);
    if (inset != nullptr) {
        return *inset;
    }
    else {
        return Frame{};
    }
}


void RichEdit::SetInset(const Frame& inset) {

    GetPropertyMap().SetProperty(kInsetPropertyName, inset);

    if (text_service_ != nullptr) {
        text_service_->OnTxPropertyBitsChange(TXTBIT_VIEWINSETCHANGE, TXTBIT_VIEWINSETCHANGE);
    }
}


std::uint32_t RichEdit::GetMaxLength() const {

    auto max_length = GetPropertyMap().TryGetProperty<std::uint32_t>(kMaxLengthPropertyName);
    if (max_length != nullptr) {
        return *max_length;
    }
    else {
        return kDefaultMaxLength;
    }
}


void RichEdit::SetMaxLength(std::uint32_t max_length) {

    GetPropertyMap().SetProperty(kMaxLengthPropertyName, max_length);

    if (text_service_ != nullptr) {
        text_service_->OnTxPropertyBitsChange(TXTBIT_MAXLENGTHCHANGE, TXTBIT_MAXLENGTHCHANGE);
    }
}


bool RichEdit::UsePasswordCharacter() const {
    return HasPropertyBit(TXTBIT_USEPASSWORD);
}

void RichEdit::SetUsePasswordCharacter(bool use_password_char) {
    ChangePropertyBit(TXTBIT_USEPASSWORD, use_password_char);
}


wchar_t RichEdit::GetPasswordCharacter() const {
    
    auto password_char = GetPropertyMap().TryGetProperty<wchar_t>(kPasswordCharacterPropertyName);
    if (password_char != nullptr) {
        return *password_char;
    }
    else {
        return kDefaultPasswordCharacter;
    }
}

void RichEdit::SetPasswordCharacter(wchar_t password_char) {

    GetPropertyMap().SetProperty(kPasswordCharacterPropertyName, password_char);

    if ((text_service_ != nullptr) && UsePasswordCharacter()) {
        text_service_->OnTxPropertyBitsChange(TXTBIT_USEPASSWORD, TXTBIT_USEPASSWORD);
    }
}


bool RichEdit::IsMultiline() const {
    return HasPropertyBit(TXTBIT_MULTILINE);
}

void RichEdit::SetIsMultiline(bool is_multiline) {
    ChangePropertyBit(TXTBIT_MULTILINE, is_multiline);
}


bool RichEdit::IsReadOnly() const {
    return HasPropertyBit(TXTBIT_READONLY);
}

void RichEdit::SetIsReadOnly(bool is_read_only) {
    ChangePropertyBit(TXTBIT_READONLY, is_read_only);
    NeedRepaint();
}


bool RichEdit::AllowBeep() const {
    return HasPropertyBit(TXTBIT_ALLOWBEEP);
}

void RichEdit::SetAllowBeep(bool allow_beep) {
    ChangePropertyBit(TXTBIT_ALLOWBEEP, allow_beep);
}


TextRange RichEdit::GetSelectionRange() const {

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

void RichEdit::SetSelectionRange(const TextRange& range) {

    if (text_service_ == nullptr) {
        return;
    }

    CHARRANGE char_range = { 0 };
    char_range.cpMin = static_cast<LONG>(range.index);
    char_range.cpMax = static_cast<LONG>(range.index + range.length);
    text_service_->TxSendMessage(EM_EXSETSEL, 0, reinterpret_cast<LPARAM>(&char_range), nullptr);
}


std::wstring RichEdit::Text() const {

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

void RichEdit::SetText(const std::wstring& text) {

    if (text_service_ != nullptr) {

        text_service_->TxSetText(text.c_str());

        ReleaseTextLayout();

        //Sometimes the text service would not require to repaint after
        //setting text, so do it here.
        NeedRepaint();
    }
}


TextValidator RichEdit::GetTextValidator() const {

    auto validator = GetPropertyMap().TryGetProperty<TextValidator>(kTextValidatorPropertyName);
    if (validator != nullptr) {
        return *validator;
    }

    return nullptr;
}

void RichEdit::SetTextValidator(const TextValidator& validator) {

    GetPropertyMap().SetProperty(kTextValidatorPropertyName, validator);
}


zaf::Font RichEdit::Font() const {

    zaf::Font font;
    font.family_name = character_format_.szFaceName;
    font.size = static_cast<float>(character_format_.yHeight) / 15;
    font.weight = (character_format_.dwEffects & CFE_BOLD) ? FontWeight::Bold : FontWeight::Regular;
    return font;
}

void RichEdit::SetFont(const zaf::Font& font) {

    ResetCachedTextHeight();

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


TextAlignment RichEdit::TextAlignment() const {

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

void RichEdit::SetTextAlignment(zaf::TextAlignment alignment) {

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
            ZAF_ALERT();
            break;
    }

    if (text_service_ != nullptr) {
        text_service_->OnTxPropertyBitsChange(TXTBIT_PARAFORMATCHANGE, TXTBIT_PARAFORMATCHANGE);
    }
}


WordWrapping RichEdit::WordWrapping() const {
    return HasPropertyBit(TXTBIT_WORDWRAP) ? WordWrapping::Wrap : WordWrapping::NoWrap;
}

void RichEdit::SetWordWrapping(zaf::WordWrapping word_wrapping) {
    ChangePropertyBit(TXTBIT_WORDWRAP, word_wrapping != WordWrapping::NoWrap);
}


bool RichEdit::HasPropertyBit(DWORD bit) const {
    return (property_bits_ & bit) != 0;
}


void RichEdit::ChangePropertyBit(DWORD bit, bool is_set) {

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


void RichEdit::SetAllowVerticalScroll(bool allow) {
    ChangeScrollBarPropertyBits(ES_AUTOVSCROLL | WS_VSCROLL, allow);
}


void RichEdit::SetAllowHorizontalScroll(bool allow) {
    ChangeScrollBarPropertyBits(ES_AUTOHSCROLL | WS_HSCROLL, allow);
}


void RichEdit::SetAutoHideScrollBars(bool auto_hide) {
    ChangeScrollBarPropertyBits(ES_DISABLENOSCROLL, ! auto_hide);
}


void RichEdit::ChangeScrollBarPropertyBits(DWORD bits, bool is_set) {

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


bool RichEdit::CanShowVerticalScrollBar() {
    return CanEnableVerticalScrollBar() || (scroll_bar_property_ & ES_DISABLENOSCROLL);
}


bool RichEdit::CanShowHorizontalScrollBar() {
    return CanEnableHorizontalScrollBar() || (scroll_bar_property_ & ES_DISABLENOSCROLL);
}


bool RichEdit::CanEnableVerticalScrollBar() {

    BOOL is_enabled = FALSE;

    if (text_service_ != nullptr) {
        text_service_->TxGetVScroll(nullptr, nullptr, nullptr, nullptr, &is_enabled);
    }

    return (is_enabled != FALSE);
}


bool RichEdit::CanEnableHorizontalScrollBar() {

    BOOL is_enabled = FALSE;

    if (text_service_ != nullptr) {
        text_service_->TxGetHScroll(nullptr, nullptr, nullptr, nullptr, &is_enabled);
    }

    return (is_enabled != FALSE);
}


void RichEdit::GetVerticalScrollValues(
    int& current_value, 
    int& min_value,
    int& max_value,
    int& page_value) {

    GetScrollValues(false, current_value, min_value, max_value, page_value);
}


void RichEdit::GetHorizontalScrollValues(
    int& current_value, 
    int& min_value,
    int& max_value,
    int& page_value) {

    GetScrollValues(true, current_value, min_value, max_value, page_value);
}


void RichEdit::GetScrollValues(
    bool is_horizontal, 
    int& current_value, 
    int& min_value, 
    int& max_value,
    int& page_value) {

    if (!text_service_) {
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

    if (FAILED(result)) {
        return;
    }

    //If the rich edit is too small, page might be a negative value, which is not allowed.
    page = std::max(page, 0L);

    current_value = current;
    min_value = min;
    max_value = std::max(max - page, 0L);
    page_value = page;
}


Observable<RichEditSelectionChangedInfo> RichEdit::SelectionChangedEvent() {
    return GetEventObservable<RichEditSelectionChangedInfo>(
        GetPropertyMap(), 
        kSelectionChangedEventPropertyName);
}


Observable<SelfScrollingControlScrollBarChangInfo> RichEdit::ScrollBarChangeEvent() {

    return GetEventObservable<SelfScrollingControlScrollBarChangInfo>(
        GetPropertyMap(),
        kScrollBarChangeEventPropertyName);
}


Observable<SelfScrollingControlScrollValuesChangeInfo> RichEdit::ScrollValuesChangeEvent() {

    return GetEventObservable<SelfScrollingControlScrollValuesChangeInfo>(
        GetPropertyMap(),
        kScrollValuesChangeEventPropertyName);
}


bool RichEdit::AcceptKeyMessage(const KeyMessage& message) {

    switch (message.VirtualKey()) {
    case VK_TAB:
        return AcceptTab();

    case VK_RETURN:
        return AcceptReturn();

    default:
        return __super::AcceptKeyMessage(message);
    }
}


bool RichEdit::AcceptTab() const {

    auto accept_tab = GetPropertyMap().TryGetProperty<bool>(kAcceptTabPropertyName);
    if (accept_tab != nullptr) {
        return *accept_tab;
    }
    else {
        return false;
    }
}

void RichEdit::SetAcceptTab(bool accept_tab) {
    GetPropertyMap().SetProperty(kAcceptTabPropertyName, accept_tab);
}


bool RichEdit::AcceptReturn() const {

    auto accept_return = GetPropertyMap().TryGetProperty<bool>(kAcceptReturnPropertyName);
    if (accept_return != nullptr) {
        return *accept_return;
    }
    else {
        return false;
    }
}

void RichEdit::SetAcceptReturn(bool accept_return) {
    GetPropertyMap().SetProperty(kAcceptReturnPropertyName, accept_return);
}


void RichEdit::OnMouseCursorChanging(const MouseCursorChangingInfo& event_info) {

    __super::OnMouseCursorChanging(event_info);
    if (event_info.IsHandled()) {
        return;
    }

    HandleMouseCursorChanging(event_info);
}


void RichEdit::HandleMouseCursorChanging(const MouseCursorChangingInfo& event_info) {

    //Don't change mouse cursor if the mouse is not in content rect.
    auto mouse_position_in_control = this->GetMousePosition();
    if (!this->ContentRect().Contain(mouse_position_in_control)) {
        return;
    }

    //Rich edit will not change mouse cursor if the mouse position is above rich edit, so we adjust
    //mouse position to the top of rich edit if it is in vertical offset area.
    mouse_position_in_control = AdjustMousePositionIntoRichEdit(mouse_position_in_control);

    auto mouse_position_in_window = mouse_position_in_control;
    mouse_position_in_window.AddOffset(this->AbsoluteRect().position);

    //Try to change mouse cursor with objects first.
    auto object_info = rich_edit::internal::OLEHelper::FindObjectUnderMouse(*this);
    if (object_info.object) {

        object_info.object->OnMouseCursorChanging(rich_edit::MouseCursorChangingContext{
            object_info.text_position,
            object_info.is_in_selection_range,
            object_info.mouse_position_in_object,
            event_info
        });

        if (event_info.IsHandled()) {
            return;
        }
    }

    Point mouse_position_in_rich_edit = FromDIPs(mouse_position_in_window, this->GetDPI());
    HRESULT result = text_service_->OnTxSetCursor(
        DVASPECT_CONTENT,
        0,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        static_cast<INT>(mouse_position_in_rich_edit.x),
        static_cast<INT>(mouse_position_in_rich_edit.y)
    );

    if (SUCCEEDED(result)) {
        event_info.MarkAsHandled();
    }
}


void RichEdit::OnMouseMove(const MouseMoveInfo& event_info) {

    __super::OnMouseMove(event_info);

    if (event_info.IsHandled()) {
        return;
    }

    const auto& message = event_info.Message();
    text_service_->TxSendMessage(message.ID(), message.WParam(), message.LParam(), nullptr);
    event_info.MarkAsHandled();
}


void RichEdit::OnMouseDown(const MouseDownInfo& event_info) {

    __super::OnMouseDown(event_info);
    if (event_info.IsHandled()) {
        return;
    }

    SetIsFocused(true);

    auto object_info = rich_edit::internal::OLEHelper::FindObjectUnderMouse(*this);
    if (object_info.object) {
        
        object_info.object->OnMouseDown(rich_edit::MouseDownContext{
            object_info.text_position,
            object_info.is_in_selection_range,
            object_info.mouse_position_in_object,
            event_info
        });

        if (event_info.IsHandled()) {
            return;
        }
    }

    const auto& message = event_info.Message();
    text_service_->TxSendMessage(message.ID(), message.WParam(), message.LParam(), nullptr);
    event_info.MarkAsHandled();
}


void RichEdit::OnMouseUp(const MouseUpInfo& event_info) {

    __super::OnMouseUp(event_info);
    if (event_info.IsHandled()) {
        return;
    }

    auto object_info = rich_edit::internal::OLEHelper::FindObjectUnderMouse(*this);
    if (object_info.object) {

        object_info.object->OnMouseUp(rich_edit::MouseUpContext{
            object_info.text_position,
            object_info.is_in_selection_range,
            object_info.mouse_position_in_object,
            event_info
        });

        if (event_info.IsHandled()) {
            return;
        }
    }

    const auto& message = event_info.Message();
    text_service_->TxSendMessage(message.ID(), message.WParam(), message.LParam(), nullptr);
    event_info.MarkAsHandled();
}


Point RichEdit::AdjustMousePositionIntoRichEdit(const Point& position_in_control) {

    //There is a vertical offset to the position of rich edit if ParagraphAlignemnt is set to 
    //Center or Bottom. In order to make mouse operations within the offset area apply to rich 
    //edit, we modify the position as if it is in the area of rich edit.

    //No offset, no need to adjust.
    float vertical_offset = GetContentVerticalOffset();
    if (vertical_offset == 0) {
        return position_in_control;
    }

    zaf::Rect offset_area_rect = ContentRect();
    offset_area_rect.size.height = vertical_offset;

    //Position is not in offset area, no need to adjust.
    if (!offset_area_rect.Contain(position_in_control)) {
        return position_in_control;
    }

    //Position is in offset area, adjust to the top of rich edit area.
    Point result = position_in_control;
    result.y = offset_area_rect.Bottom();
    return result;
}


void RichEdit::OnKeyDown(const KeyDownInfo& event_info) {
    
    __super::OnKeyDown(event_info);

    if (event_info.IsHandled()) {
        return;
    }

    if (!text_service_) {
        return;
    }

    HRESULT result = text_service_->TxSendMessage(
        WM_KEYDOWN, 
        event_info.Message().WParam(),
        event_info.Message().LParam(),
        nullptr);

    event_info.MarkAsHandled();
}


void RichEdit::OnKeyUp(const KeyUpInfo& event_info) {

    __super::OnKeyUp(event_info);

    if (event_info.IsHandled()) {
        return;
    }

    if (!text_service_) {
        return;
    }

    HRESULT result = text_service_->TxSendMessage(
        WM_KEYUP, 
        event_info.Message().WParam(),
        event_info.Message().LParam(),
        nullptr);

    event_info.MarkAsHandled();
}


void RichEdit::OnCharInput(const CharInputInfo& event_info) {

    __super::OnCharInput(event_info);

    if (event_info.IsHandled()) {
        return;
    }

    if (!text_service_) {
        return;
    }

    HRESULT result = text_service_->TxSendMessage(
        WM_CHAR, 
        event_info.Message().WParam(),
        event_info.Message().LParam(),
        nullptr);

    event_info.MarkAsHandled();
}


void RichEdit::OnFocusGained(const FocusGainedInfo& event_info) {

    __super::OnFocusGained(event_info);

    if (event_info.IsHandled()) {
        return;
    }

    if (!text_service_) {
        return;
    }

    text_service_->TxSendMessage(WM_SETFOCUS, 0, 0, nullptr);
    event_info.MarkAsHandled();
}


void RichEdit::OnFocusLost(const FocusLostInfo& event_info) {

    __super::OnFocusLost(event_info);

    if (event_info.IsHandled()) {
        return;
    }

    if (!text_service_) {
        return;
    }

    if (caret_) {
        caret_->SetIsVisible(false);
    }

    text_service_->TxSendMessage(WM_KILLFOCUS, 0, 0, nullptr);
    event_info.MarkAsHandled();
}


bool RichEdit::CanUndo() const {

    if (text_service_ != nullptr) {
        LRESULT can_undo = FALSE;
        text_service_->TxSendMessage(EM_CANUNDO, 0, 0, &can_undo);
        return can_undo != FALSE;
    }
    else {
        return false;
    }
}


bool RichEdit::Undo() {

    if (text_service_ != nullptr) {
        LRESULT undo_result = FALSE;
        text_service_->TxSendMessage(EM_UNDO, 0, 0, &undo_result);
        return undo_result != FALSE;
    }
    else {
        return false;
    }
}


bool RichEdit::CanRedo() const {

    if (text_service_ != nullptr) {
        LRESULT can_redo = FALSE;
        text_service_->TxSendMessage(EM_CANREDO, 0, 0, &can_redo);
        return can_redo != FALSE;
    }
    else {
        return false;
    }
}


bool RichEdit::Redo() {

    if (text_service_ != nullptr) {
        LRESULT redo_result = FALSE;
        text_service_->TxSendMessage(EM_REDO, 0, 0, &redo_result);
        return redo_result != FALSE;
    }
    else {
        return false;
    }
}


void RichEdit::ScrollUpByLine() {
    SendScrollMessage(false, SB_LINEUP);
}


void RichEdit::ScrollDownByLine() {
    SendScrollMessage(false, SB_LINEDOWN);
}


void RichEdit::ScrollUpByPage() {
    SendScrollMessage(false, SB_PAGEUP);
}


void RichEdit::ScrollDownByPage() {
    SendScrollMessage(false, SB_PAGEDOWN);
}


void RichEdit::ScrollUpToBegin() {
    SendScrollMessage(false, SB_TOP);
}


void RichEdit::ScrollDownToEnd() {
    SendScrollMessage(false, SB_BOTTOM);
}


void RichEdit::ScrollLeftToBegin() {
    SendScrollMessage(true, SB_LEFT);
}


void RichEdit::ScrollRightToEnd() {

    //Don't use WM_HSCROLL with SB_RIGHT, because it would scroll "too far",
    //not the result expected.
    int current_value = 0;
    int min_value = 0;
    int max_value = 0;
    int page_value = 0;
    GetScrollValues(true, current_value, min_value, max_value, page_value);
    HorizontallyScroll(max_value);

    //Sometimes the text service would not require to repaint after scrolling,
    //so do it here.
    NeedRepaint();
}


void RichEdit::SendScrollMessage(bool is_horizontal, WORD scroll_type) {

    if (text_service_ != nullptr) {
        text_service_->TxSendMessage(is_horizontal ? WM_HSCROLL : WM_VSCROLL, MAKEWPARAM(scroll_type, 0), 0, nullptr);
    }
}


void RichEdit::VerticallyScroll(int new_value) {
    Scroll(false, new_value);
}


void RichEdit::HorizontallyScroll(int new_value) {
    Scroll(true, new_value);
}


void RichEdit::Scroll(bool is_horizontal, int new_value) {

    if (text_service_ == nullptr) {
        return;
    }

    UINT message = is_horizontal ? WM_HSCROLL : WM_VSCROLL;
    text_service_->TxSendMessage(message, MAKEWPARAM(SB_THUMBPOSITION, new_value), 0, nullptr);
}


void RichEdit::ScrollBarChange() {

    auto event_observer = GetEventObserver<SelfScrollingControlScrollBarChangInfo>(
        GetPropertyMap(),
        kScrollBarChangeEventPropertyName);

    if (event_observer) {

        SelfScrollingControlScrollBarChangInfo event_info;
        event_info.self_scrolling_control = this;
        event_observer->OnNext(event_info);
    }
}


void RichEdit::ScrollValuesChange(bool is_horizontal) {

    auto event_observer = GetEventObserver<SelfScrollingControlScrollValuesChangeInfo>(
        GetPropertyMap(),
        kScrollValuesChangeEventPropertyName);

    if (event_observer) {

        SelfScrollingControlScrollValuesChangeInfo event_info;
        event_info.self_scrolling_control = this;
        event_info.is_horizontal = is_horizontal;
        event_observer->OnNext(event_info);
    }
}


void RichEdit::HandleSelectionChangedNotification() {
    OnSelectionChanged(RichEditSelectionChangedInfo{ As<RichEdit>(shared_from_this()) });
}


void RichEdit::OnSelectionChanged(const RichEditSelectionChangedInfo& event_info) {

    auto event_observer = GetEventObserver<RichEditSelectionChangedInfo>(
        GetPropertyMap(),
        kSelectionChangedEventPropertyName);

    if (event_observer) {
        event_observer->OnNext(event_info);
    }
}


void RichEdit::HandleTextChangedNotification() {

    //In order to make text changing event work, we rely on EN_PROTECTED notification. But the
    //protected text range isn't extended automatically by rich edit when text changed, so we 
    //update the protected text range to the whole text ourselves.
    CHARFORMAT char_format{};
    char_format.cbSize = sizeof(char_format);
    char_format.dwMask = CFM_PROTECTED;
    char_format.dwEffects = CFE_PROTECTED;
    text_service_->TxSendMessage(
        EM_SETCHARFORMAT, 
        SCF_ALL,
        reinterpret_cast<LPARAM>(&char_format),
        nullptr);

    NotifyTextChanged();
}


void RichEdit::InsertObject(const COMObject<rich_edit::EmbeddedObject>& object) {

    ZAF_EXPECT(object.IsValid());

    auto ole_interface = GetOLEInterface();

    COMObject<IOleClientSite> client_site;
    HRESULT hresult = ole_interface->GetClientSite(client_site.Store());
    ZAF_THROW_IF_COM_ERROR(hresult);

    REOBJECT object_info{};
    object_info.cbStruct = sizeof(object_info);
    object_info.clsid = object.Inner()->ClassID();
    object_info.poleobj = object.Inner();
    object_info.polesite = client_site.Inner();
    object_info.pstg = nullptr;
    object_info.cp = REO_CP_SELECTION;
    object_info.dvaspect = DVASPECT_CONTENT;
    object_info.dwFlags = REO_BELOWBASELINE | REO_OWNERDRAWSELECT;

    auto object_size = FromDIPs(object.Inner()->Size(), this->GetDPI()).ToSIZEL();
    AtlPixelToHiMetric(&object_size, &object_info.sizel);

    hresult = ole_interface->InsertObject(&object_info);
    ZAF_THROW_IF_COM_ERROR(hresult);
}


COMObject<IRichEditOle> RichEdit::GetOLEInterface() const {

    COMObject<IRichEditOle> result{};
    HRESULT hresult = text_service_->TxSendMessage(
        EM_GETOLEINTERFACE,
        0,
        reinterpret_cast<LPARAM>(result.Store()),
        nullptr);

    ZAF_THROW_IF_COM_ERROR(hresult);
    return result;
}

}