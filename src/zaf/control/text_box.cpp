#include <zaf/control/text_box.h>
#include <zaf/base/as.h>
#include <zaf/base/log.h>
#include <zaf/control/caret.h>
#include <zaf/control/internal/textual_control/text_box_core.h>
#include <zaf/graphic/canvas.h>
#include <zaf/object/type_definition.h>

namespace zaf {
namespace {

Rect GetCaretRect(const DWRITE_HIT_TEST_METRICS& metrics) {
    return Rect{ metrics.left, metrics.top, 1, metrics.height };
}

}

ZAF_DEFINE_TYPE(TextBox)
ZAF_DEFINE_TYPE_END

TextBox::TextBox() : TextualControl(std::make_unique<internal::TextBoxCore>()) {

}


void TextBox::Initialize() {

    __super::Initialize();

    core_ = As<internal::TextBoxCore>(Core());

    SetCanFocused(true);
    SetCanTabStop(true);
    SetBackgroundColor(Color::White());

    caret_ = zaf::Create<zaf::Caret>(As<TextBox>(shared_from_this()));
}


std::wstring_view TextBox::GetText() const {
    return std::get<0>(core_->GetText());
}


TextLayout TextBox::GetTextLayout() const {
    return core_->GetTextLayout();
}


void TextBox::Paint(Canvas& canvas, const zaf::Rect& dirty_rect) {

    __super::Paint(canvas, dirty_rect);

    auto region_guard = canvas.PushRegion(ContentRect(), dirty_rect);
    caret_->Paint(canvas, dirty_rect);
}


void TextBox::PaintSelectionBackground(Canvas& canvas, const zaf::Rect& dirty_rect) {

    if (selection_range_.length == 0) {
        return;
    }

    auto text_layout = GetTextLayout();

    UINT32 metrics_count{};
    HRESULT hresult = text_layout.Inner()->HitTestTextRange(
        static_cast<UINT32>(selection_range_.index),
        static_cast<UINT32>(selection_range_.length),
        0,
        0,
        nullptr,
        0,
        &metrics_count);

    if (hresult != E_NOT_SUFFICIENT_BUFFER) {
        return;
    }

    auto buffer = std::make_unique<DWRITE_HIT_TEST_METRICS[]>(metrics_count);
    hresult = text_layout.Inner()->HitTestTextRange(
        static_cast<UINT32>(selection_range_.index),
        static_cast<UINT32>(selection_range_.length),
        0,
        0,
        buffer.get(),
        metrics_count,
        &metrics_count);

    if (FAILED(hresult)) {
        return;
    }

    auto brush = canvas.Renderer().CreateSolidColorBrush(zaf::Color::FromARGB(0x7FAADCFF));

    auto text = std::get<0>(core_->GetText());

    for (UINT32 index = 0; index < metrics_count; ++index) {

        const auto& metrics = buffer.get()[index];
        
        zaf::Rect rect{ metrics.left, metrics.top, metrics.width, metrics.height };

        //Draw extra space to represent line breaks
        auto last_index = metrics.textPosition + metrics.length - 1;
        if (text[last_index] == L'\n' || text[last_index] == L'\r') {
            rect.size.width += metrics.height / 3;
        }

        canvas.DrawRectangle(rect, brush);
    }
}


void TextBox::OnMouseCursorChanging(const MouseCursorChangingInfo& event_info) {

    __super::OnMouseCursorChanging(event_info);

    if (event_info.IsHandled()) {
        return;
    }

    SetCursor(LoadCursor(nullptr, IDC_IBEAM));
    event_info.MarkAsHandled();
}


void TextBox::OnMouseDown(const MouseDownInfo& event_info) {

    __super::OnMouseDown(event_info);
    if (event_info.IsHandled()) {
        return;
    }

    HandleMouseDown(event_info);
    event_info.MarkAsHandled();
}


void TextBox::HandleMouseDown(const MouseDownInfo& event_info) {

    this->SetIsFocused(true);

    auto index_info = FindTextIndexAtPoint(event_info.PositionAtSource());
    if (!index_info) {
        return;
    }

    CaptureMouse();

    UpdateSelectionByMouse(*index_info, true);
    NeedRepaint();
}


void TextBox::OnMouseMove(const MouseMoveInfo& event_info) {

    __super::OnMouseMove(event_info);
    if (event_info.IsHandled()) {
        return;
    }

    HandleMouseMove(event_info);
    event_info.MarkAsHandled();
}


void TextBox::HandleMouseMove(const MouseMoveInfo& event_info) {

    if (!begin_mouse_select_index_) {
        return;
    }

    auto index_info = FindTextIndexAtPoint(event_info.PositionAtSource());
    if (!index_info) {
        return;
    }

    UpdateSelectionByMouse(*index_info, false);
    NeedRepaint();
}


void TextBox::OnMouseUp(const MouseUpInfo& event_info) {

    __super::OnMouseUp(event_info);
    if (event_info.IsHandled()) {
        return;
    }

    HandleMouseUp(event_info);
    event_info.MarkAsHandled();
}


void TextBox::HandleMouseUp(const MouseUpInfo& event_info) {

    if (!begin_mouse_select_index_) {
        return;
    }

    ReleaseMouse();

    auto index_info = FindTextIndexAtPoint(event_info.PositionAtSource());
    if (index_info) {
        UpdateSelectionByMouse(*index_info, false);
    }

    begin_mouse_select_index_.reset();
    NeedRepaint();
}


std::optional<TextBox::TextIndexInfo> TextBox::FindTextIndexAtPoint(const Point& point) const {

    auto text_layout = GetTextLayout();

    BOOL is_tailing{};
    BOOL is_inside{};
    DWRITE_HIT_TEST_METRICS metrics{};
    HRESULT hresult = text_layout.Inner()->HitTestPoint(
        point.x,
        point.y,
        &is_tailing,
        &is_inside,
        &metrics);

    if (FAILED(hresult)) {
        return std::nullopt;
    }

    TextIndexInfo result;
    result.index = metrics.textPosition;
    result.rect = GetCaretRect(metrics);
    if (is_tailing) {
        ++result.index;
        result.rect.position.x += metrics.width;
    }
    return result;
}


void TextBox::UpdateSelectionByMouse(const TextIndexInfo& index_info, bool begin_selection) {

    if (begin_selection) {
        begin_mouse_select_index_ = index_info.index;
        caret_index_ = index_info.index;
    }
    else {
        if (!begin_mouse_select_index_) {
            return;
        }
        caret_index_ = index_info.index;
    }

    begin_key_select_index_.reset();

    selection_range_ = Range::FromIndexPair(
        (std::min)(*begin_mouse_select_index_, caret_index_),
        (std::max)(*begin_mouse_select_index_, caret_index_));

    ShowCaret(index_info.rect);
}


void TextBox::OnFocusGained(const FocusGainedInfo& event_info) {

    __super::OnFocusGained(event_info);
    if (event_info.IsHandled()) {
        return;
    }

    UpdateCaretAtCurrentIndex();
    event_info.MarkAsHandled();
}


void TextBox::OnFocusLost(const FocusLostInfo& event_info) {

    __super::OnFocusLost(event_info);
    if (event_info.IsHandled()) {
        return;
    }

    caret_->SetIsVisible(false);
    event_info.MarkAsHandled();
}


void TextBox::OnKeyDown(const KeyDownInfo& event_info) {

    __super::OnKeyDown(event_info);
    if (event_info.IsHandled()) {
        return;
    }

    bool is_selecting_range = !!(GetKeyState(VK_SHIFT) >> 15);

    auto virtual_key = event_info.Message().VirtualKey();
    if (virtual_key == VK_LEFT) {
        BackwardCaretIndex(is_selecting_range);
    }
    else if (virtual_key == VK_RIGHT) {
        ForwardCaretIndex(is_selecting_range);
    }
}


void TextBox::BackwardCaretIndex(bool is_selecting_range) {

    std::size_t new_index = caret_index_;
    if (new_index > 0) {

        --new_index;

        if (new_index > 0) {

            //Skip CRLF line break.
            auto text = GetText();
            if (text[new_index - 1] == L'\r' && text[new_index] == L'\n') {

                --new_index;
            }
        }
    }

    UpdateCaretIndex(new_index, is_selecting_range);
}


void TextBox::ForwardCaretIndex(bool is_selecting_range) {

    auto text = GetText();

    std::size_t new_index = caret_index_;
    if (new_index < text.length()) {

        ++new_index;

        if (new_index < text.length()) {

            //Skip CRLF line break.
            if (text[caret_index_] == L'\r' && text[new_index] == L'\n') {
                ++new_index;
            }
        }
    }

    UpdateCaretIndex(new_index, is_selecting_range);
}


void TextBox::UpdateCaretIndex(std::size_t new_index, bool is_selecting_range) {

    if (is_selecting_range) {
        if (!begin_key_select_index_) {
            begin_key_select_index_ = caret_index_;
        }
    }
    else {
        begin_key_select_index_.reset();
    }

    caret_index_ = (std::min)(new_index, core_->GetTextLength());

    auto begin_select_index = begin_key_select_index_.value_or(caret_index_);
    selection_range_ = Range::FromIndexPair(
        (std::min)(begin_select_index, caret_index_),
        (std::max)(begin_select_index, caret_index_));
    
    UpdateCaretAtCurrentIndex();
    NeedRepaint();
}


void TextBox::UpdateCaretAtCurrentIndex() {

    FLOAT x{};
    FLOAT y{};
    DWRITE_HIT_TEST_METRICS metrics{};
    HRESULT hresult = GetTextLayout().Inner()->HitTestTextPosition(
        static_cast<UINT32>(caret_index_),
        FALSE,
        &x,
        &y,
        &metrics);

    if (SUCCEEDED(hresult)) {
        ShowCaret(GetCaretRect(metrics));
    }
}


void TextBox::ShowCaret(const zaf::Rect& caret_rect) {

    caret_->SetRect(Align(caret_rect));
    caret_->SetIsVisible(true);
}

}