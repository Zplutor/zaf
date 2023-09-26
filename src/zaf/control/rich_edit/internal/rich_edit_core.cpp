#include <zaf/control/rich_edit/internal/rich_edit_core.h>
#include <zaf/base/as.h>
#include <zaf/internal/theme.h>

namespace zaf::rich_edit::internal {

void RichEditCore::Initialize(const std::shared_ptr<TextualControl>& owner) {
    owner_ = As<RichEdit>(owner);
}


std::size_t RichEditCore::GetTextLength() {

    auto owner = owner_.lock();
    if (!owner) {
        return 0;
    }

    GETTEXTLENGTHEX param{};
    param.flags = GTL_DEFAULT | GTL_PRECISE | GTL_NUMCHARS;
    param.codepage = 1200; //Unicode

    LRESULT length{};
    owner->text_service_->TxSendMessage(
        EM_GETTEXTLENGTHEX,
        reinterpret_cast<WPARAM>(&param),
        0,
        &length);

    return length;
}


std::variant<std::wstring_view, std::wstring> RichEditCore::GetText() {

    auto owner = owner_.lock();
    if (!owner) {
        return {};
    }

    std::wstring text;
    BSTR text_buffer{};
    HRESULT result = owner->text_service_->TxGetText(&text_buffer);
    if (SUCCEEDED(result) && text_buffer) {

        text.assign(text_buffer);
        SysFreeString(text_buffer);
    }
    return text;
}


zaf::internal::SetTextResult RichEditCore::SetText(const std::wstring& text) {

    auto owner = owner_.lock();
    if (!owner) {
        return {};
    }

    owner->text_service_->TxSetText(text.c_str());

    zaf::internal::SetTextResult result;
    result.is_changed = true;
    //Rich edit will send text changed notification itself.
    result.is_notification_sent = true;
    return result;
}


Font RichEditCore::GetFont() {

    auto owner = owner_.lock();
    if (!owner) {
        return {};
    }

    zaf::Font font;
    font.family_name = owner->character_format_.szFaceName;
    font.size = static_cast<float>(owner->character_format_.yHeight) / 15;
    font.weight = 
        (owner->character_format_.dwEffects & CFE_BOLD) ?
        FontWeight::Bold :
        FontWeight::Regular;

    return font;
}


void RichEditCore::SetFont(const Font& font) {

    auto owner = owner_.lock();
    if (!owner) {
        return;
    }

    owner->ResetCachedTextHeight();

    owner->character_format_.dwMask |= CFM_FACE;
    wcscpy_s(owner->character_format_.szFaceName, font.family_name.c_str());

    owner->character_format_.dwMask |= CFM_SIZE;
    owner->character_format_.yHeight = static_cast<LONG>(font.size * 15);

    owner->character_format_.dwMask |= CFM_BOLD;
    if (font.weight > FontWeight::Bold) {
        owner->character_format_.dwEffects |= CFE_BOLD;
    }
    else {
        owner->character_format_.dwEffects &= ~CFE_BOLD;
    }

    if (owner->text_service_) {
        owner->text_service_->OnTxPropertyBitsChange(
            TXTBIT_CHARFORMATCHANGE,
            TXTBIT_CHARFORMATCHANGE);
    }
}


Font RichEditCore::GetFontAtPosition(std::size_t position) {
    //Unsupported, always returns default font.
    return GetFont();
}


void RichEditCore::SetFontAtRange(const zaf::Font& font, const Range& range) {
    //Unsupported.
}


void RichEditCore::ResetFonts() {
    //Unsupported.
}


ColorPicker RichEditCore::GetTextColorPicker() {

    if (text_color_picker_) {
        return text_color_picker_;
    }

    return [](const Control& control) {
        if (control.IsEnabled()) {
            return Color::FromRGB(zaf::internal::ControlNormalTextColorRGB);
        }
        else {
            return Color::FromRGB(zaf::internal::ControlDisabledTextColorRGB);
        }
    };
}


void RichEditCore::SetTextColorPicker(const ColorPicker& color_picker) {

    text_color_picker_ = color_picker;

    auto owner = owner_.lock();
    if (owner) {
        owner->NeedRepaint();
    }
}


ColorPicker RichEditCore::GetTextColorPickerAtPosition(std::size_t position) {
    //Unsupported. Use the default text color.
    return GetTextColorPicker();
}


void RichEditCore::SetTextColorPickerAtRange(const ColorPicker& color_picker, const Range& range) {
    //Unsupported.
}


void RichEditCore::ResetTextColorPickers() {
    //Unsupported.
}


TextAlignment RichEditCore::GetTextAlignment() {

    auto owner = owner_.lock();
    if (!owner) {
        return {};
    }

    switch (owner->paragraph_format_.wAlignment) {
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


void RichEditCore::SetTextAlignment(TextAlignment alignment) {

    auto owner = owner_.lock();
    if (!owner) {
        return;
    }

    owner->paragraph_format_.dwMask |= PFM_ALIGNMENT;

    switch (alignment) {
    case TextAlignment::Center:
        owner->paragraph_format_.wAlignment = PFA_CENTER;
        break;
    case TextAlignment::Leading:
        owner->paragraph_format_.wAlignment = PFA_LEFT;
        break;
    case TextAlignment::Tailing:
        owner->paragraph_format_.wAlignment = PFA_RIGHT;
        break;
    default:
        ZAF_ALERT();
        break;
    }

    if (owner->text_service_) {
        owner->text_service_->OnTxPropertyBitsChange(
            TXTBIT_PARAFORMATCHANGE,
            TXTBIT_PARAFORMATCHANGE);
    }
}


ParagraphAlignment RichEditCore::GetParagraphAlignment() {
    return paragraph_alignment_;
}


void RichEditCore::SetParagraphAlignment(ParagraphAlignment alignment) {

    paragraph_alignment_ = alignment;

    auto owner = owner_.lock();
    if (owner) {
        owner->ResetCachedTextHeight();
        owner->NeedRepaint();
    }
}


WordWrapping RichEditCore::GetWordWrapping() {

    auto owner = owner_.lock();
    if (!owner) {
        return {};
    }

    return owner->HasPropertyBit(TXTBIT_WORDWRAP) ? WordWrapping::Wrap : WordWrapping::NoWrap;
}


void RichEditCore::SetWordWrapping(WordWrapping word_wrapping) {

    auto owner = owner_.lock();
    if (!owner) {
        return;
    }

    owner->ResetCachedTextHeight();
    owner->ChangePropertyBit(TXTBIT_WORDWRAP, word_wrapping != WordWrapping::NoWrap);
}


TextTrimming RichEditCore::GetTextTrimming() {
    //Unsupported.
    return {};
}


void RichEditCore::SetTextTrimming(const zaf::TextTrimming& text_trimming) {
    //Unsupported.
}


LineSpacing RichEditCore::GetLineSpacing() {
    return {};
}


void RichEditCore::SetLineSpacing(const LineSpacing& line_spacing) {
    //Unsupported.
}


bool RichEditCore::IgnoreTailingWhiteSpaces() const {
    return false;
}


void RichEditCore::SetIgnoreTailingWhiteSpaces(bool value) {
    //Unsupported.
}


Size RichEditCore::CalculateTextSize(const Size& boundary_size) {
    //Nothing to do. We calculate the size in RichEdit.
    return {};
}


void RichEditCore::LayoutText(const Rect& text_boundary) {
    //Nothing to do.
}


void RichEditCore::PaintText(Canvas& canvas, const Rect& dirty_rect)  {
    //Nothing to do. We paint the text in RichEdit.
}


void RichEditCore::ReleaseRendererResources() {
    //Nothing to do.
}

}