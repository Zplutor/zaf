#include <zaf/control/text_box.h>
#include <zaf/base/as.h>
#include <zaf/base/log.h>
#include <zaf/control/caret.h>
#include <zaf/control/textual/dynamic_inline_object.h>
#include <zaf/input/mouse.h>
#include <zaf/internal/textual/text_model.h>
#include <zaf/internal/textual/text_box_editor.h>
#include <zaf/internal/textual/text_box_hit_test_manager.h>
#include <zaf/internal/textual/text_box_keyboard_input_handler.h>
#include <zaf/internal/textual/text_box_module_context.h>
#include <zaf/internal/textual/text_box_mouse_input_handler.h>
#include <zaf/internal/textual/text_box_selection_manager.h>
#include <zaf/graphic/canvas.h>
#include <zaf/object/type_definition.h>

namespace zaf {

ZAF_DEFINE_TYPE(TextBox)
ZAF_DEFINE_TYPE_PROPERTY(AllowUndo)
ZAF_DEFINE_TYPE_PROPERTY(IsEditable)
ZAF_DEFINE_TYPE_PROPERTY(SelectedText)
ZAF_DEFINE_TYPE_PROPERTY(SelectionRange)
ZAF_DEFINE_TYPE_END

TextBox::TextBox() : word_extractor_(textual::DefaultWordExtractor()) {

}


TextBox::~TextBox() {

}


void TextBox::Initialize() {

    __super::Initialize();

    module_context_ = std::make_unique<internal::TextBoxModuleContext>(this);
    module_context_->Initialize();

    Subscriptions() += module_context_->SelectionManager().SelectionChangedEvent().Subscribe(
        std::bind(&TextBox::OnSelectionChanged, this, std::placeholders::_1));

    SetCanFocused(true);
    SetCanTabStop(true);
    SetCanDoubleClick(true);
    SetBackgroundColor(Color::White());

    caret_ = zaf::Create<zaf::Caret>(As<TextBox>(shared_from_this()));
}


void TextBox::Layout(const zaf::Rect& previous_rect) {

    //Text rect need to be updated before calling Layout(), as Layout() relies on text rect.
    UpdateTextRectOnLayout();

    __super::Layout(previous_rect);

    if (caret_->IsVisible()) {
        UpdateCaretAtCurrentIndex();
    }
}


void TextBox::UpdateTextRectOnLayout() {

    constexpr auto update_single_dimension = [](
        float content_length, 
        float metrics_length,
        bool allow_scroll,
        float& text_position, 
        float& text_length) {
    
        if ((content_length >= metrics_length) || !allow_scroll) {
            text_length = content_length;
            text_position = 0;
        }
        else {
            text_length = metrics_length;
            auto text_end_position = text_position + text_length;
            if (text_end_position < content_length) {
                text_position += content_length - text_end_position;
            }
        }
    };

    auto old_text_rect = text_rect_;
    auto content_size = ContentSize();
    auto text_size = CalculatePreferredContentSize(content_size);

    //Update x and width.
    update_single_dimension(
        content_size.width, 
        text_size.width,
        allow_horizontal_scroll_ && (WordWrapping() == WordWrapping::NoWrap),
        text_rect_.position.x,
        text_rect_.size.width);

    //Update y and height.
    update_single_dimension(
        content_size.height,
        text_size.height,
        allow_vertical_scroll_,
        text_rect_.position.y,
        text_rect_.size.height);

    //Notify scroll bars are changed if text rect is changed.
    if (text_rect_ != old_text_rect) {
        SelfScrollControlScrollBarChangeInfo event_info{ this };
        scroll_bar_change_event_.Raise(event_info);
    }
}


zaf::Rect TextBox::DetermineTextRect() {
    return text_rect_;
}


ColorPicker TextBox::SelectionBackgroundColorPicker() const {

    if (selection_background_color_picker_) {
        return selection_background_color_picker_;
    }

    return [](const Control& control) {

        if (control.ContainFocus()) {
            //Use the same color as rich edit.
            auto color = zaf::Color::FromCOLORREF(GetSysColor(COLOR_HIGHLIGHT));
            color.a = 100.f / 255.f;
            return color;
        }

        return Color::FromRGB(0xE4E4E4);
    };
}

void TextBox::SetSelectionBackgroundColorPicker(ColorPicker picker) {
    selection_background_color_picker_ = std::move(picker);
    NeedRepaint();
}


Color TextBox::SelectionBackgroundColor() const {
    return SelectionBackgroundColorPicker()(*this);
}

void TextBox::SetSelectionBackgroundColor(const Color& color) {
    SetSelectionBackgroundColorPicker(CreateColorPicker(color));
}


void TextBox::Paint(Canvas& canvas, const zaf::Rect& dirty_rect) {

    __super::Paint(canvas, dirty_rect);

    PaintCaret(canvas, dirty_rect);
}


void TextBox::PaintTextLayout(
    Canvas& canvas,
    const zaf::Rect& dirty_rect,
    const TextLayout& text_layout, 
    const zaf::Rect& text_layout_rect) {

    //Paint background before painting text.
    PaintTextBackground(canvas, dirty_rect, text_layout, text_layout_rect);

    __super::PaintTextLayout(canvas, dirty_rect, text_layout, text_layout_rect);
}


void TextBox::PaintTextBackground(
    Canvas& canvas,
    const zaf::Rect& dirty_rect,
    const TextLayout& text_layout,
    const zaf::Rect& layout_rect) {

    auto selection_range = this->SelectionRange();
    if (selection_range.length == 0) {
        return;
    }

    auto metrics_list = text_layout.HitTestRange(selection_range);
    if (metrics_list.empty()) {
        return;
    }

    auto region_guard = canvas.PushRegion(layout_rect, layout_rect);

    auto background_color = SelectionBackgroundColor();
    auto brush = canvas.Renderer().CreateSolidColorBrush(background_color);
    std::wstring_view text = TextModel().GetText();

    for (const auto& metrics : metrics_list) {

        zaf::Rect rect = metrics.Rect();

        //Draw extra space to represent line breaks
        auto last_index = metrics.TextIndex() + metrics.Length() - 1;
        if (text[last_index] == L'\n' || text[last_index] == L'\r') {
            rect.size.width += metrics.Height() / 3;
        }

        canvas.DrawRectangle(rect, brush);
    }
}


void TextBox::PaintCaret(Canvas& canvas, const zaf::Rect& dirty_rect) {

    auto content_rect = ContentRect();
    auto region_guard = canvas.PushRegion(content_rect, dirty_rect);

    auto caret_dirty_rect = dirty_rect;
    caret_dirty_rect.SubtractOffset(content_rect.position);
    caret_->Paint(canvas, caret_dirty_rect);
}


bool TextBox::IsEditable() const {
    return module_context_->Editor().CanEdit();
}


void TextBox::SetIsEditable(bool is_editable) {
    module_context_->Editor().SetCanEdit(is_editable);
}


bool TextBox::AllowUndo() const {
    return module_context_->Editor().AllowUndo();
}


void TextBox::SetAllowUndo(bool allow_undo) {
    module_context_->Editor().SetAllowUndo(allow_undo);
}


bool TextBox::CanUndo() const {
    return module_context_->Editor().CanUndo();
}


bool TextBox::Undo() {
    return module_context_->Editor().Undo();
}


bool TextBox::CanRedo() const {
    return module_context_->Editor().CanRedo();
}


bool TextBox::Redo() {
    return module_context_->Editor().Redo();
}


void TextBox::OnMouseCursorChanging(const MouseCursorChangingInfo& event_info) {

    __super::OnMouseCursorChanging(event_info);
    if (event_info.IsHandled()) {
        return;
    }

    module_context_->MouseInputHandler().HandleMouseCursorChanging(event_info);
    if (event_info.IsHandled()) {
        return;
    }

    Mouse::SetCursor(Cursor::IBeam());
    event_info.MarkAsHandled();
}


void TextBox::OnMouseDown(const MouseDownInfo& event_info) {

    __super::OnMouseDown(event_info);
    if (event_info.IsHandled()) {
        return;
    }

    module_context_->MouseInputHandler().HandleMouseDown(event_info);
    event_info.MarkAsHandled();
}


void TextBox::OnMouseMove(const MouseMoveInfo& event_info) {

    __super::OnMouseMove(event_info);
    if (event_info.IsHandled()) {
        return;
    }

    module_context_->MouseInputHandler().HandleMouseMove(event_info);
    event_info.MarkAsHandled();
}


void TextBox::OnMouseUp(const MouseUpInfo& event_info) {

    __super::OnMouseUp(event_info);
    if (event_info.IsHandled()) {
        return;
    }

    module_context_->MouseInputHandler().HandleMouseUp(event_info);
    event_info.MarkAsHandled();
}


std::size_t TextBox::FindIndexAtPosition(const Point& position) const {

    const auto& hit_test_manager = module_context_->HitTestManager();
    auto hit_test_result = hit_test_manager.HitTestAtPosition(position);
    return hit_test_manager.TextIndexFromHitTestResult(hit_test_result);
}


bool TextBox::IsPositionInsideText(const Point& position) const {
    return module_context_->HitTestManager().HitTestAtPosition(position).IsInside();
}


void TextBox::OnDoubleClick(const DoubleClickInfo& event_info) {

    __super::OnDoubleClick(event_info);

    module_context_->MouseInputHandler().HandleDoubleClick(event_info);
}


void TextBox::SelectWordAtIndex(std::size_t index, textual::SelectionOption selection_option) {

    auto word_range = word_extractor_(TextModel().GetText(), index);
    SetSelectionRange(word_range, selection_option);
}


const textual::WordExtractor& TextBox::WordExtractor() const noexcept {
    return word_extractor_;
}


void TextBox::SetWordExtractor(textual::WordExtractor extractor) noexcept {
    if (extractor) {
        word_extractor_ = std::move(extractor);
    }
    else {
        word_extractor_ = textual::DefaultWordExtractor();
    }
}


void TextBox::OnFocusGained(const FocusGainedInfo& event_info) {

    __super::OnFocusGained(event_info);

    UpdateCaretAtCurrentIndex();
    event_info.MarkAsHandled();

    if (this->SelectionRange().length > 0) {
        NeedRepaint();
    }
}


void TextBox::OnFocusLost(const FocusLostInfo& event_info) {

    __super::OnFocusLost(event_info);

    caret_->SetIsVisible(false);
    event_info.MarkAsHandled();

    if (this->SelectionRange().length > 0) {
        NeedRepaint();
    }
}


void TextBox::OnKeyDown(const KeyDownInfo& event_info) {

    __super::OnKeyDown(event_info);
    if (event_info.IsHandled()) {
        return;
    }

    module_context_->KeyboardInputHandler().HandleKeyDown(event_info);
    event_info.MarkAsHandled();
}


void TextBox::OnCharInput(const CharInputInfo& event_info) {

    __super::OnCharInput(event_info);
    if (event_info.IsHandled()) {
        return;
    }

    module_context_->Editor().HandleCharInput(event_info);
    event_info.MarkAsHandled();
}


void TextBox::OnTextChanged(const TextChangedInfo& event_info) {

    //Clear previous text rect and re-calulate it.
    text_rect_ = {};
    NeedRelayout();

    //Clear the selection range if it's not editing.
    if (!module_context_->Editor().IsEditing()) {
        SetSelectionRange(Range{});
    }

    __super::OnTextChanged(event_info);
}


std::size_t TextBox::CaretIndex() const {
    return module_context_->SelectionManager().CaretIndex();
}


const Range& TextBox::SelectionRange() const {
    return module_context_->SelectionManager().SelectionRange();
}


void TextBox::SetSelectionRange(const Range& range, textual::SelectionOption selection_option) {

    std::wstring_view text = module_context_->TextModel().GetText();

    auto revised_range = Range::FromIndexPair(
        (std::min)(range.index, text.length()),
        (std::min)(range.EndIndex(), text.length()));

    module_context_->SelectionManager().SetSelectionRange(
        revised_range,
        selection_option, 
        std::nullopt,
        true);
}


std::wstring TextBox::SelectedText() const {

    std::wstring_view text = TextModel().GetText();
    auto selection_range = this->SelectionRange();
    auto selected_text = text.substr(selection_range.index, selection_range.length);
    return std::wstring{ selected_text };
}


void TextBox::OnSelectionChanged(const internal::TextBoxSelectionChangedInfo& event_info) {

    if (event_info.NeedScrollToCaret()) {
        EnsureCaretVisible(event_info.CharRectAtCaret());
    }

    ShowCaret(event_info.CharRectAtCaret());
    NeedRepaint();
}


void TextBox::UpdateCaretAtCurrentIndex() {

    auto hit_test_result = GetTextLayout().HitTestIndex(this->CaretIndex(), false);
    ShowCaret(hit_test_result.Metrics().Rect());
}


void TextBox::ShowCaret(const zaf::Rect& char_rect_at_caret) {

    zaf::Rect caret_rect = char_rect_at_caret;
    caret_rect.size.width = 1;
    caret_rect.AddOffset(text_rect_.position);

    caret_->SetRect(ToPixelAligned(caret_rect, this->GetDPI()));

    if (this->IsFocused()) {
        caret_->SetIsVisible(true);
    }
}


void TextBox::EnsureCaretVisible(const zaf::Rect& char_rect_at_caret) {

    constexpr auto update_single_dimension = [](
        float content_length,
        float metrics_position, 
        float metrics_length,
        float& text_position) {
    
        float begin = metrics_position + text_position;
        if (begin < 0) {
            text_position -= begin;
            return true;
        }
        else {
            float end = metrics_position + metrics_length + text_position;
            if (end >= content_length) {
                text_position += content_length - end;
                return true;
            }
        }
        return false;
    };

    auto content_size = ContentSize();
    if (content_size.width <= 0 || content_size.height <= 0) {
        return;
    }

    //Update x.
    auto x_changed = update_single_dimension(
        content_size.width,
        char_rect_at_caret.Left(),
        char_rect_at_caret.size.width,
        text_rect_.position.x);

    //Update y.
    auto y_changed = update_single_dimension(
        content_size.height, 
        char_rect_at_caret.Top(),
        char_rect_at_caret.size.height,
        text_rect_.position.y);

    if (x_changed) {
        SelfScrollControlScrollValuesChangeInfo event_info;
        event_info.self_scroll_control = this;
        event_info.is_horizontal = true;
        scroll_values_change_event_.Raise(event_info);
    }

    if (y_changed) {
        SelfScrollControlScrollValuesChangeInfo event_info;
        event_info.self_scroll_control = this;
        event_info.is_horizontal = false;
        scroll_values_change_event_.Raise(event_info);
    }

    if (x_changed || y_changed) {
        UpdateTextRect(text_rect_);
    }
}


void TextBox::SetAllowVerticalScroll(bool allow) {

    allow_vertical_scroll_ = allow;
    NeedRelayout();
    NeedRepaint();
}


void TextBox::SetAllowHorizontalScroll(bool allow) {

    allow_horizontal_scroll_ = allow;
    NeedRelayout();
    NeedRepaint();
}


void TextBox::SetAutoHideScrollBars(bool auto_hide) {
    auto_hide_scroll_bars_ = auto_hide;
    NeedRelayout();
    NeedRepaint();
}


bool TextBox::CanShowVerticalScrollBar() {

    if (!allow_vertical_scroll_) {
        return false;
    }

    if (!CanEnableVerticalScrollBar()) {
        return !auto_hide_scroll_bars_;
    }

    return true;
}


bool TextBox::CanShowHorizontalScrollBar() {
    
    if (!allow_horizontal_scroll_) {
        return false;
    }

    if (!CanEnableHorizontalScrollBar()) {
        return !auto_hide_scroll_bars_;
    }

    return true;
}


bool TextBox::CanEnableVerticalScrollBar() {
    return ContentSize().height < text_rect_.size.height;
}


bool TextBox::CanEnableHorizontalScrollBar() {
    return ContentSize().width < text_rect_.size.width;
}


void TextBox::GetVerticalScrollValues(
    int& current_value, 
    int& min_value,
    int& max_value,
    int& page_value) {

    GetScrollValues(
        ContentSize().height,
        text_rect_.size.height,
        text_rect_.position.y,
        current_value, 
        min_value, 
        max_value, 
        page_value);
}


void TextBox::GetHorizontalScrollValues(
    int& current_value, 
    int& min_value,
    int& max_value,
    int& page_value) {

    GetScrollValues(
        ContentSize().width,
        text_rect_.size.width,
        text_rect_.position.x,
        current_value,
        min_value,
        max_value,
        page_value);
}


void TextBox::GetScrollValues(
    float content_length,
    float text_length,
    float text_position,
    int& current_value,
    int& min_value,
    int& max_value,
    int& page_value) {

    min_value = 0;

    float size_difference = text_length - content_length;
    max_value = static_cast<int>(size_difference);

    current_value = static_cast<int>(-text_position);
    page_value = static_cast<int>(content_length);
}


Observable<SelfScrollControlScrollBarChangeInfo> TextBox::ScrollBarChangeEvent() {
    return scroll_bar_change_event_.GetObservable();
}


Observable<SelfScrollControlScrollValuesChangeInfo> TextBox::ScrollValuesChangeEvent() {
    return scroll_values_change_event_.GetObservable();
}


void TextBox::VerticallyScroll(int new_value) {
    DoScroll(new_value, ContentSize().height, text_rect_.size.height, text_rect_.position.y);
}


void TextBox::HorizontallyScroll(int new_value) {
    DoScroll(new_value, ContentSize().width, text_rect_.size.width, text_rect_.position.x);
}


void TextBox::DoScroll(
    int new_value, 
    float content_length, 
    float text_length, 
    float& text_position) {

    float difference = text_length - content_length;
    int rounded_difference = static_cast<int>(difference);

    int revised_value = (std::max)(0, new_value);
    revised_value = (std::min)(revised_value, rounded_difference);

    if (revised_value == rounded_difference) {
        text_position = -difference;
    }
    else {
        text_position = static_cast<float>(-revised_value);
    }

    UpdateTextRect(text_rect_);

    if (caret_->IsVisible()) {
        UpdateCaretAtCurrentIndex();
    }
    NeedRepaint();
}


std::size_t TextBox::LineCount() const {
    return GetTextLayout().GetMetrics().LineCount();
}

}