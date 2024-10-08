#include <zaf/control/rich_edit.h>
#include <atlwin.h>
#include <tom.h>
#include <cassert>
#include <zaf/base/error/check.h>
#include <zaf/base/error/unknown_runtime_error.h>
#include <zaf/base/error/win32_error.h>
#include <zaf/base/log.h>
#include <zaf/control/caret.h>
#include <zaf/control/control_object.h>
#include <zaf/control/rich_edit/embedded_object.h>
#include <zaf/control/rich_edit/internal/ole_callback_impl.h>
#include <zaf/control/rich_edit/internal/ole_helper.h>
#include <zaf/control/rich_edit/internal/ole_object_impl.h>
#include <zaf/control/rich_edit/internal/text_host_bridge.h>
#include <zaf/graphic/alignment.h>
#include <zaf/graphic/canvas.h>
#include <zaf/graphic/dpi.h>
#include <zaf/graphic/d2d/renderer.h>
#include <zaf/graphic/font.h>
#include <zaf/internal/theme.h>
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

COMPtr<ITextServices2> CreateTextService(ITextHost* text_host) {

    HMODULE module_handle = LoadLibrary(L"msftedit.dll");
    if (!module_handle) {
        ZAF_THROW_WIN32_ERROR(GetLastError());
    }

    using CreateTextServicesFunction = HRESULT(_stdcall*)(IUnknown*, ITextHost*, IUnknown**);
    auto create_function = reinterpret_cast<CreateTextServicesFunction>(
        GetProcAddress(module_handle, "CreateTextServices"));

    if (!create_function) {
        ZAF_THROW_WIN32_ERROR(GetLastError());
    }

    auto iid_text_service2 = reinterpret_cast<const IID*>(
        GetProcAddress(module_handle, "IID_ITextServices2"));

    if (!iid_text_service2) {
        ZAF_THROW_WIN32_ERROR(GetLastError());
    }

    COMPtr<IUnknown> unknown;
    HRESULT hresult = create_function(nullptr, text_host, unknown.Reset());
    ZAF_THROW_IF_COM_ERROR(hresult);

    COMPtr<ITextServices2> text_service;
    hresult = unknown->QueryInterface(*iid_text_service2, text_service.ResetAsVoid());
    ZAF_THROW_IF_COM_ERROR(hresult);

    return text_service;
}

constexpr DWORD kDefaultPropertyBits = TXTBIT_D2DDWRITE | TXTBIT_ALLOWBEEP;
constexpr DWORD kDefaultScrollBarProperty = 
    ES_AUTOVSCROLL | ES_AUTOHSCROLL | WS_VSCROLL | WS_HSCROLL;

}

ZAF_OBJECT_IMPL(RichEdit);


RichEdit::RichEdit() : 
    property_bits_(kDefaultPropertyBits),
    character_format_(),
    paragraph_format_(),
    scroll_bar_property_(kDefaultScrollBarProperty) {

    character_format_.cbSize = sizeof(character_format_);
    character_format_.dwMask |= CFM_PROTECTED;
    character_format_.dwEffects |= CFE_PROTECTED;

    paragraph_format_.cbSize = sizeof(paragraph_format_);
}


RichEdit::~RichEdit() {

}


void RichEdit::Initialize() {

    __super::Initialize();

    SetCanFocused(true);
    SetCanDoubleClick(true);
    SetBorder(Frame(1));

    //Initialize CHARFORMATW and PARAFORMAT.
    SetFont(Font::Default());
    SetTextAlignment(dwrite::TextAlignment::Leading);
    SetParagraphAlignment(dwrite::ParagraphAlignment::Near);

    SetBorderColor(Color::Black());

    SetTextColorPicker(ColorPicker([](const Control& control) {
        return control.IsEnabledInContext() ?
            Color::FromRGB(zaf::internal::ControlNormalTextColorRGB) :
            Color::FromRGB(zaf::internal::ControlDisabledTextColorRGB);
    }));

    SetBackgroundColorPicker(ColorPicker([](const Control& control) {
        const auto& edit = As<RichEdit>(control);
        if (edit.IsReadOnly() || !edit.IsEnabledInContext()) {
            return Color::FromRGB(0xEEEEEE);;
        }
        return Color::White();
    }));

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

    if (this->Size() != previous_rect.size) {
        ResetCachedTextHeight();
    }

    if (text_service_ != nullptr) {
        text_service_->OnTxPropertyBitsChange(TXTBIT_CLIENTRECTCHANGE, TXTBIT_CLIENTRECTCHANGE);
    }
}


zaf::Size RichEdit::CalculatePreferredContentSize(const zaf::Size& max_size) const {

    auto max_size_in_pixel = FromDIPs(max_size, GetDPI());

    auto float_to_long = [](float f) -> LONG {
        //The magic number of max width that TxGetNaturalSize() can accept without problem.
        //If rich edit enables multi-line and word wrapping, and the width passed to
        //TxGetNaturalSize() is greater than this number, an incorrect result will be returned.
        constexpr LONG Max = 286331152;
        if (f > Max) {
            return Max;
        }
        else {
            return static_cast<LONG>(f);
        }
    };

    LONG width = float_to_long(max_size_in_pixel.width);

    // Height must be 0 in order to get accurate height.
    LONG height = 0; 

    //Use -1,-1 to disable zooming.
    SIZE extend{ -1, -1 };

    HRESULT hresult = text_service_->TxGetNaturalSize(
        DVASPECT_CONTENT,
        nullptr,
        nullptr,
        nullptr,
        TXTNS_FITTOCONTENT3 | TXTNS_INCLUDELASTLINE,
        &extend,
        &width,
        &height);

    if (FAILED(hresult)) {
        return {};
    }

    zaf::Size result{ static_cast<float>(width), static_cast<float>(height) };
    result = ToDIPs(result, this->GetDPI());
    return result;
}


void RichEdit::UpdateStyle() {

    __super::UpdateStyle();

    text_color_field_.UpdateColor(*this);
    ReviseTextColor();
}


void RichEdit::Paint(Canvas& canvas, const zaf::Rect& dirty_rect) const {

    __super::Paint(canvas, dirty_rect);

    //Note: all painting operations are in content coordinate.
    auto content_rect = this->ContentRect();
    auto content_region_guard = canvas.PushRegion(content_rect, dirty_rect);

    zaf::Rect bounds_in_content;
    bounds_in_content.position.y = GetContentVerticalOffset();
    bounds_in_content.size = content_rect.size;
    bounds_in_content = ToPixelAligned(bounds_in_content, this->GetDPI());
    auto bounds_rect = FromDIPs(bounds_in_content, this->GetDPI()).ToRECTL();

    zaf::Rect update_area_in_content = dirty_rect;
    update_area_in_content.SubtractOffset(content_rect.position);
    update_area_in_content = ToPixelAligned(update_area_in_content, this->GetDPI());
    auto update_rect = FromDIPs(update_area_in_content, this->GetDPI()).ToRECT();

    text_service_->TxDrawD2D(
        canvas.Renderer().Ptr().Inner(),
        &bounds_rect,
        &update_rect,
        TXTVIEW_ACTIVE);

    PaintEmbeddedObjects(canvas, update_area_in_content);

    if (caret_) {
        caret_->Paint(canvas, update_area_in_content);
    }
}


void RichEdit::PaintEmbeddedObjects(Canvas& canvas, const zaf::Rect& dirty_rect) const {

    try {

        auto ole_interface = GetOLEInterface();

        auto object_count = ole_interface.GetObjectCount();
        if (object_count <= 0) {
            return;
        }

        auto text_document = ole_interface.Ptr().Query<ITextDocument>();
        if (!text_document) {
            return;
        }

        auto absolute_content_rect = this->ContentRectInWindow();
        if (!absolute_content_rect) {
            return;
        }

        auto selection_range = this->GetSelectionRange();

        for (std::size_t index = 0; index < object_count; ++index) {

            auto object_info = ole_interface.GetObjectInfoAt(index);

            auto embedded_object = rich_edit::EmbeddedObject::TryFromCOMPtr(object_info.Object());
            if (!embedded_object) {
                continue;
            }

            COMPtr<ITextRange> text_range;
            HRESULT hresult = text_document->Range(
                static_cast<long>(object_info.Index()),
                static_cast<long>(object_info.Index()),
                text_range.Reset());

            if (FAILED(hresult)) {
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

            Point object_position{ 
                static_cast<float>(absolute_x), 
                static_cast<float>(absolute_y) 
            };
            object_position = ToDIPs(object_position, this->GetDPI());
            object_position.SubtractOffset(absolute_content_rect->position);

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
                    static_cast<std::size_t>(object_info.Index()),
                    selection_range.Contains(object_info.Index())
                }
            );
        }
    }
    catch (const zaf::Error&) {

    }
}


float RichEdit::GetContentVerticalOffset() const {

    auto paragraph_alignment = ParagraphAlignment();
    if (paragraph_alignment == dwrite::ParagraphAlignment::Near) {
        return 0;
    }

    if (IsMultiline()) {

        //For multi-line rich edit, don't use offset if there is vertical scroll bar.
        int current{};
        int min{};
        int max{};
        int page{};
        this->GetVerticalScrollValues(current, min, max, page);

        bool has_vertical_bar = (min != max);
        if (has_vertical_bar) {
            return 0;
        }
    }

    if (!cached_text_height_) {

        auto preferred_size = CalculatePreferredContentSize(zaf::Size{ 
            this->ContentSize().width,
            std::numeric_limits<float>::max()
        });

        cached_text_height_ = preferred_size.height;
    }

    if (paragraph_alignment == dwrite::ParagraphAlignment::Center) {
        return (ContentSize().height - *cached_text_height_) / 2;
    }
    else if (paragraph_alignment == dwrite::ParagraphAlignment::Far) {
        return ContentSize().height - *cached_text_height_;
    }
    else {
        return 0;
    }
}


void RichEdit::ResetCachedTextHeight() {
    cached_text_height_.reset();
}


std::size_t RichEdit::TextLength() const {

    GETTEXTLENGTHEX param{};
    param.flags = GTL_DEFAULT | GTL_PRECISE | GTL_NUMCHARS;
    param.codepage = 1200; //Unicode

    LRESULT length{};
    text_service_->TxSendMessage(
        EM_GETTEXTLENGTHEX,
        reinterpret_cast<WPARAM>(&param),
        0,
        &length);

    return length;
}


std::wstring RichEdit::Text() const {

    std::wstring text;
    BSTR text_buffer{};
    HRESULT result = text_service_->TxGetText(&text_buffer);
    if (SUCCEEDED(result) && text_buffer) {

        text.assign(text_buffer);
        SysFreeString(text_buffer);
    }
    return text;
}


void RichEdit::SetText(const std::wstring& text) {
    text_service_->TxSetText(text.c_str());
}


std::wstring RichEdit::GetText(rich_edit::TextFlag flag) const {

    constexpr UINT CodePage = 1200; //Unicode code page.

    GETTEXTLENGTHEX text_length_options{};
    text_length_options.codepage = CodePage;
    text_length_options.flags = 
        GTL_NUMCHARS | GTL_CLOSE | 
        (flag == rich_edit::TextFlag::UseCRLF ? GTL_USECRLF : GTL_DEFAULT);

    LRESULT text_length_in_chars{};
    HRESULT hresult = text_service_->TxSendMessage(
        EM_GETTEXTLENGTHEX, 
        reinterpret_cast<WPARAM>(&text_length_options),
        0,
        &text_length_in_chars);

    if (FAILED(hresult) || text_length_in_chars == E_INVALIDARG) {
        return {};
    }

    //Text length returned from EM_GETTEXTLENGTHEX doesn't contain the null terminator, and the 
    //buffer passed to EM_GETTEXTEX is required to contain null terminator, so we add one more 
    //char to text length.
    text_length_in_chars++;
    std::wstring text(text_length_in_chars, L'\0');

    GETTEXTEX text_options{};
    text_options.cb = static_cast<DWORD>(text_length_in_chars * sizeof(wchar_t));
    text_options.codepage = CodePage;
    text_options.flags = (flag == rich_edit::TextFlag::UseCRLF ? GT_USECRLF : GT_DEFAULT);

    LRESULT actual_length_in_chars{};
    hresult = text_service_->TxSendMessage(
        EM_GETTEXTEX, 
        reinterpret_cast<WPARAM>(&text_options),
        reinterpret_cast<LPARAM>(&text[0]),
        &actual_length_in_chars);

    if (FAILED(hresult)) {
        return {};
    }

    text.resize(actual_length_in_chars);
    return text;
}


std::wstring RichEdit::GetTextInRange(const Range& range) const {

    //EM_RANGETEXT returns inconsistent string with other text getting methods, like WCH_EMBEDDING
    //chars will be replaced to spaces. So we use ITextDocument instead.

    auto text_document = this->GetOLEInterface().Ptr().Query<ITextDocument>();
    if (!text_document) {
        return {};
    }

    zaf::COMPtr<ITextRange> text_range;
    HRESULT hresult = text_document->Range(
        static_cast<long>(range.index),
        static_cast<long>(range.index + range.length),
        text_range.Reset());

    if (FAILED(hresult)) {
        return {};
    }

    BSTR bstr{};
    hresult = text_range->GetText(&bstr);
    if (FAILED(hresult)) {
        return {};
    }

    std::wstring result{ bstr };
    SysFreeString(bstr);
    return result;
}


Font RichEdit::Font() const {

    zaf::Font font;
    font.family_name = character_format_.szFaceName;
    font.size = static_cast<float>(character_format_.yHeight) / 15;
    font.weight =
        (character_format_.dwEffects & CFE_BOLD) ?
        FontWeight::Bold :
        FontWeight::Regular;

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

    if (text_service_) {
        text_service_->OnTxPropertyBitsChange(
            TXTBIT_CHARFORMATCHANGE,
            TXTBIT_CHARFORMATCHANGE);
    }
}


std::wstring RichEdit::FontFamily() const {
    return Font().family_name;
}


void RichEdit::SetFontFamily(const std::wstring& family) {

    auto font = Font();
    font.family_name = family;
    SetFont(font);
}


float RichEdit::FontSize() const {
    return Font().size;
}


void RichEdit::SetFontSize(float size) {

    auto font = Font();
    font.size = size;
    SetFont(font);
}


FontWeight RichEdit::FontWeight() const {
    return Font().weight;
}


void RichEdit::SetFontWeight(zaf::FontWeight weight) {

    auto font = Font();
    font.weight = weight;
    SetFont(font);
}


dwrite::TextAlignment RichEdit::TextAlignment() const {

    switch (paragraph_format_.wAlignment) {
    case PFA_CENTER:
        return dwrite::TextAlignment::Center;
    case PFA_LEFT:
        return dwrite::TextAlignment::Leading;
    case PFA_RIGHT:
        return dwrite::TextAlignment::Tailing;
    default:
        return dwrite::TextAlignment::Leading;
    }
}


void RichEdit::SetTextAlignment(zaf::dwrite::TextAlignment alignment) {

    paragraph_format_.dwMask |= PFM_ALIGNMENT;

    switch (alignment) {
    case dwrite::TextAlignment::Center:
        paragraph_format_.wAlignment = PFA_CENTER;
        break;
    case dwrite::TextAlignment::Leading:
        paragraph_format_.wAlignment = PFA_LEFT;
        break;
    case dwrite::TextAlignment::Tailing:
        paragraph_format_.wAlignment = PFA_RIGHT;
        break;
    default:
        ZAF_ALERT();
        break;
    }

    if (text_service_) {
        text_service_->OnTxPropertyBitsChange(
            TXTBIT_PARAFORMATCHANGE,
            TXTBIT_PARAFORMATCHANGE);
    }
}


dwrite::ParagraphAlignment RichEdit::ParagraphAlignment() const {
    return paragraph_alignment_;
}


void RichEdit::SetParagraphAlignment(dwrite::ParagraphAlignment alignment) {

    paragraph_alignment_ = alignment;

    ResetCachedTextHeight();
    NeedRepaint();
}


dwrite::WordWrapping RichEdit::WordWrapping() const {
    return 
        HasPropertyBit(TXTBIT_WORDWRAP) ? 
        dwrite::WordWrapping::Wrap : 
        dwrite::WordWrapping::NoWrap;
}


void RichEdit::SetWordWrapping(dwrite::WordWrapping word_wrapping) {

    ResetCachedTextHeight();
    ChangePropertyBit(TXTBIT_WORDWRAP, word_wrapping != dwrite::WordWrapping::NoWrap);
}


Color RichEdit::TextColor() const {
    return text_color_field_.Color();
}


void RichEdit::SetTextColor(const Color& color) {
    text_color_field_.SetColor(color, *this);
    ReviseTextColor();
}


const ColorPicker& RichEdit::TextColorPicker() const {
    return text_color_field_.ColorPicker();
}

void RichEdit::SetTextColorPicker(ColorPicker picker) {
    text_color_field_.SetColorPicker(std::move(picker), *this);
    ReviseTextColor();
}


void RichEdit::ReviseTextColor() {

    character_format_.dwMask |= CFM_COLOR;
    character_format_.crTextColor = TextColor().ToCOLORREF();

    if (text_service_) {
        text_service_->OnTxPropertyBitsChange(TXTBIT_CHARFORMATCHANGE, TXTBIT_CHARFORMATCHANGE);
    }
}


Frame RichEdit::GetInset() const {
    return inset_;
}


void RichEdit::SetInset(const Frame& inset) {

    inset_ = inset;

    if (text_service_ != nullptr) {
        text_service_->OnTxPropertyBitsChange(TXTBIT_VIEWINSETCHANGE, TXTBIT_VIEWINSETCHANGE);
    }
}


std::uint32_t RichEdit::GetMaxLength() const {
    return max_length_;
}


void RichEdit::SetMaxLength(std::uint32_t max_length) {

    max_length_ = max_length;

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
    return password_char_;
}

void RichEdit::SetPasswordCharacter(wchar_t password_char) {

    password_char_ = password_char;

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


std::size_t RichEdit::LineCount() const {

    //Line count is never less than 1.
    LRESULT result{ 1 };
    text_service_->TxSendMessage(EM_GETLINECOUNT, 0, 0, &result);
    return static_cast<std::size_t>(result);
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


Range RichEdit::GetSelectionRange() const {

    CHARRANGE char_range{};
    text_service_->TxSendMessage(EM_EXGETSEL, 0, reinterpret_cast<LPARAM>(&char_range), nullptr);
    return Range::FromCHARRANGE(char_range);
}

void RichEdit::SetSelectionRange(const Range& range) {

    if (text_service_ == nullptr) {
        return;
    }

    CHARRANGE char_range = range.ToCHARRANGE();
    text_service_->TxSendMessage(EM_EXSETSEL, 0, reinterpret_cast<LPARAM>(&char_range), nullptr);
}


void RichEdit::InsertText(const std::wstring& text) {

    text_service_->TxSendMessage(
        EM_REPLACESEL,
        TRUE,
        reinterpret_cast<LPARAM>(text.c_str()),
        nullptr);
}


void RichEdit::Copy() {
    text_service_->TxSendMessage(WM_COPY, 0, 0, nullptr);
}


void RichEdit::Cut() {
    text_service_->TxSendMessage(WM_CUT, 0, 0, nullptr);
}


void RichEdit::Paste() {
    text_service_->TxSendMessage(WM_PASTE, 0, 0, nullptr);
}


TextValidator RichEdit::GetTextValidator() const {
    return text_validator_;
}

void RichEdit::SetTextValidator(const TextValidator& validator) {
    text_validator_ = validator;
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
    int& page_value) const {

    GetScrollValues(false, current_value, min_value, max_value, page_value);
}


void RichEdit::GetHorizontalScrollValues(
    int& current_value, 
    int& min_value,
    int& max_value,
    int& page_value) const {

    GetScrollValues(true, current_value, min_value, max_value, page_value);
}


void RichEdit::GetScrollValues(
    bool is_horizontal, 
    int& current_value, 
    int& min_value, 
    int& max_value,
    int& page_value) const {

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


Observable<SelfScrollControlScrollBarChangeInfo> RichEdit::ScrollBarChangeEvent() {
    return scroll_bar_change_event_.GetObservable();
}


Observable<SelfScrollControlScrollValuesChangeInfo> RichEdit::ScrollValuesChangeEvent() {
    return scroll_values_change_event_.GetObservable();
}


bool RichEdit::AcceptKeyMessage(const KeyMessage& message) {

    switch (message.Key()) {
    case Key::Tab:
        return AcceptTab();

    case Key::Enter:
        return AcceptReturn();

    default:
        return __super::AcceptKeyMessage(message);
    }
}


bool RichEdit::AcceptTab() const {
    return accept_tab_;
}

void RichEdit::SetAcceptTab(bool accept_tab) {
    accept_tab_ = accept_tab;
}


bool RichEdit::AcceptReturn() const {
    return accept_return_;
}

void RichEdit::SetAcceptReturn(bool accept_return) {
    accept_return_ = accept_return;
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
    auto rect_in_window = this->RectInWindow();
    if (rect_in_window) {
        mouse_position_in_window.AddOffset(rect_in_window->position);
    }

    //Try to change mouse cursor with objects first.
    auto object_info = rich_edit::internal::OLEHelper::FindObjectUnderMouse(*this);
    if (object_info.object) {

        object_info.object->OnMouseCursorChanging(rich_edit::MouseCursorChangingContext{
            object_info.text_position,
            object_info.is_in_selection_range,
            object_info.mouse_position_in_object,
            object_info.object_position_in_screen,
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
            object_info.object_position_in_screen,
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
            object_info.object_position_in_screen,
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


void RichEdit::OnDoubleClick(const DoubleClickInfo& event_info) {

    __super::OnDoubleClick(event_info);

    auto object_info = rich_edit::internal::OLEHelper::FindObjectUnderMouse(*this);
    if (object_info.object) {

        bool is_handled = object_info.object->OnDoubleClick(rich_edit::DoubleClickContext{
            object_info.text_position,
            object_info.is_in_selection_range,
            object_info.mouse_position_in_object,
            object_info.object_position_in_screen,
            event_info
        });

        if (is_handled) {
            return;
        }
    }

    auto window = this->Window();
    if (!window) {
        return;
    }

    POINT mouse_position{};
    GetCursorPos(&mouse_position);
    ScreenToClient(window->Handle(), &mouse_position);

    text_service_->TxSendMessage(
        WM_LBUTTONDBLCLK,
        0,
        MAKELPARAM(static_cast<WORD>(mouse_position.x), static_cast<WORD>(mouse_position.y)),
        nullptr);
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


void RichEdit::OnWindowChanged(const WindowChangedInfo& event_info) {

    __super::OnWindowChanged(event_info);

    ime_message_subscription_.Unsubscribe();

    auto window = Window();
    if (!window) {
        return;
    }

    ime_message_subscription_ = window->MessageReceivedEvent().Subscribe(
        [this](const MessageReceivedInfo& event_info) {
    
        if (event_info.IsHandled()) {
            return;
        }

        if (!this->IsFocused()) {
            return;
        }

        //Forward IME messages to RichEdit.
        const auto& message = event_info.Message();
        switch (message.ID()) {
            case WM_IME_STARTCOMPOSITION:
            case WM_IME_ENDCOMPOSITION:
            case WM_IME_COMPOSITION:
            case WM_IME_COMPOSITIONFULL:
            case WM_IME_KEYDOWN:
            case WM_IME_KEYUP:
            case WM_IME_CHAR:
            case WM_IME_CONTROL:
            case WM_IME_NOTIFY:
            case WM_IME_REQUEST:
            case WM_IME_SELECT: {
                LRESULT lresult{};
                text_service_->TxSendMessage(
                    message.ID(),
                    message.WParam(),
                    message.LParam(), 
                    &lresult);
                event_info.MarkAsHandled(lresult);
                break;
            }
            default:
                break;
        }
    });
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

    SelfScrollControlScrollBarChangeInfo event_info;
    event_info.self_scroll_control = this;
    scroll_bar_change_event_.Raise(event_info);
}


void RichEdit::ScrollValuesChange(bool is_horizontal) {

    SelfScrollControlScrollValuesChangeInfo event_info;
    event_info.self_scroll_control = this;
    event_info.is_horizontal = is_horizontal;
    scroll_values_change_event_.Raise(event_info);
}


void RichEdit::HandleSelectionChangedNotification() {
    OnSelectionChanged(rich_edit::SelectionChangedInfo{ As<RichEdit>(shared_from_this()) });
}


void RichEdit::OnSelectionChanged(const rich_edit::SelectionChangedInfo& event_info) {
    selection_changed_event_.Raise(event_info);
}


Observable<rich_edit::SelectionChangedInfo> RichEdit::SelectionChangedEvent() const {
    return selection_changed_event_.GetObservable();
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

    OnTextChanged(rich_edit::TextChangedInfo{ As<RichEdit>(shared_from_this()) });
}


bool RichEdit::HandleProtectedNotification(const ENPROTECTED& notification_info) {
    return RaiseTextChangingEvent(notification_info);
}


bool RichEdit::RaiseTextChangingEvent(const ENPROTECTED& notification_info) {

    rich_edit::TextChangeReason reason{ rich_edit::TextChangeReason::Unknown };
    if (notification_info.msg == WM_CHAR) {
        reason = rich_edit::TextChangeReason::KeyInput;
    }
    else if (notification_info.msg == WM_PASTE) {
        reason = rich_edit::TextChangeReason::Paste;
    }

    Message triggered_message{
        notification_info.nmhdr.hwndFrom,
        notification_info.msg,
        notification_info.wParam,
        notification_info.lParam
    };

    rich_edit::TextChangingInfo event_info{
        As<RichEdit>(shared_from_this()),
        reason,
        Range::FromCHARRANGE(notification_info.chrg),
        triggered_message
    };

    OnTextChanging(event_info);
    return event_info.CanChange();
}


void RichEdit::OnTextChanging(const rich_edit::TextChangingInfo& event_info) {
    text_changing_event_.Raise(event_info);
}


void RichEdit::OnTextChanged(const rich_edit::TextChangedInfo& event_info) {
    text_changed_event_.Raise(event_info);
}


Observable<rich_edit::TextChangingInfo> RichEdit::TextChangingEvent() const {
    return text_changing_event_.GetObservable();
}


Observable<rich_edit::TextChangedInfo> RichEdit::TextChangedEvent() const {
    return text_changed_event_.GetObservable();
}


void RichEdit::InsertObject(std::shared_ptr<rich_edit::EmbeddedObject> object) {

    ZAF_EXPECT(object);

    auto ole_interface = GetOLEInterface();

    COMPtr<IOleClientSite> client_site;
    HRESULT hresult = ole_interface.Ptr()->GetClientSite(client_site.Reset());
    ZAF_THROW_IF_COM_ERROR(hresult);

    auto ole_object = MakeCOMPtr<rich_edit::internal::OLEObjectImpl>(object);

    REOBJECT object_info{};
    object_info.cbStruct = sizeof(object_info);
    object_info.clsid = object->ClassID();
    object_info.poleobj = ole_object.Inner();
    object_info.polesite = client_site.Inner();
    object_info.pstg = nullptr;
    object_info.cp = REO_CP_SELECTION;
    object_info.dvaspect = DVASPECT_CONTENT;
    object_info.dwFlags = REO_BELOWBASELINE | REO_OWNERDRAWSELECT;

    auto object_size = FromDIPs(object->Size(), this->GetDPI()).ToSIZEL();
    AtlPixelToHiMetric(&object_size, &object_info.sizel);

    hresult = ole_interface.Ptr()->InsertObject(&object_info);
    ZAF_THROW_IF_COM_ERROR(hresult);

    object->SetHost(As<RichEdit>(shared_from_this()));
}


rich_edit::OLEInterface RichEdit::GetOLEInterface() const {

    COMPtr<IRichEditOle> inner;
    HRESULT hresult = text_service_->TxSendMessage(
        EM_GETOLEINTERFACE,
        0,
        reinterpret_cast<LPARAM>(inner.Reset()),
        nullptr);

    ZAF_THROW_IF_COM_ERROR(hresult);
    return rich_edit::OLEInterface{ inner };
}


void RichEdit::SetOLECallback(std::weak_ptr<rich_edit::OLECallback> callback) {

    auto shared_callback = callback.lock();
    ZAF_EXPECT(shared_callback);

    shared_callback->SetHost(As<RichEdit>(shared_from_this()));
    auto ole_callback = MakeCOMPtr<rich_edit::internal::OLECallbackImpl>(std::move(callback));

    LRESULT lresult{};
    HRESULT hresult = text_service_->TxSendMessage(
        EM_SETOLECALLBACK,
        0,
        reinterpret_cast<LPARAM>(ole_callback.Inner()),
        &lresult);

    ZAF_THROW_IF_COM_ERROR(hresult);

    if (lresult == 0) {
        //Fails to set OLE callback due to internal error.
        throw UnknownRuntimeError{ ZAF_SOURCE_LOCATION() };
    }
}

}