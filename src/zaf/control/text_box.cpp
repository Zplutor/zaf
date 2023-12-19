#include <zaf/control/text_box.h>
#include <zaf/base/as.h>
#include <zaf/base/log.h>
#include <zaf/clipboard/clipboard.h>
#include <zaf/control/caret.h>
#include <zaf/control/internal/textual_control/text_box_core.h>
#include <zaf/graphic/canvas.h>
#include <zaf/object/type_definition.h>

namespace zaf {

ZAF_DEFINE_TYPE(TextBox)
ZAF_DEFINE_TYPE_PROPERTY(SelectedText)
ZAF_DEFINE_TYPE_PROPERTY(SelectionRange)
ZAF_DEFINE_TYPE_END

TextBox::TextBox() : 
    TextualControl(std::make_unique<internal::TextBoxCore>()), 
    word_extractor_(text_box::DefaultWordExtractor()) {

}


void TextBox::Initialize() {

    __super::Initialize();

    core_ = As<internal::TextBoxCore>(Core());

    SetCanFocused(true);
    SetCanTabStop(true);
    SetCanDoubleClick(true);
    SetBackgroundColor(Color::White());

    caret_ = zaf::Create<zaf::Caret>(As<TextBox>(shared_from_this()));
}


std::wstring_view TextBox::GetText() const {
    return std::get<0>(core_->GetText());
}


TextLayout TextBox::GetTextLayout() const {
    return core_->GetTextLayout();
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
    auto text_size = core_->CalculateTextSize(content_size);

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
        SelfScrollingControlScrollBarChangInfo event_info{ this };
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


void TextBox::PaintTextBackground(
    Canvas& canvas,
    const zaf::Rect& dirty_rect,
    const TextLayout& text_layout,
    const zaf::Rect& layout_rect) {

    if (selection_range_.length == 0) {
        return;
    }

    auto metrics_list = text_layout.HitTestRange(selection_range_);
    if (metrics_list.empty()) {
        return;
    }

    auto region_guard = canvas.PushRegion(layout_rect, layout_rect);

    auto background_color = SelectionBackgroundColor();
    auto brush = canvas.Renderer().CreateSolidColorBrush(background_color);
    auto text = std::get<0>(core_->GetText());

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

    auto new_index = FindIndexAtPosition(event_info.PositionAtSource());
    CaptureMouse();
    SetCaretIndexByMouse(new_index, true);
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

    auto new_index = FindIndexAtPosition(event_info.PositionAtSource());
    SetCaretIndexByMouse(new_index, false);
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

    auto new_index = FindIndexAtPosition(event_info.PositionAtSource());
    SetCaretIndexByMouse(new_index, false);

    begin_mouse_select_index_.reset();
}


std::size_t TextBox::FindIndexAtPosition(const Point& position) const {

    auto point_in_text_rect = position;
    point_in_text_rect.SubtractOffset(ContentRect().position);
    point_in_text_rect.SubtractOffset(text_rect_.position);

    auto hit_test_result = GetTextLayout().HitTestPoint(point_in_text_rect);
    std::size_t result = hit_test_result.Metrics().TextIndex();
    if (hit_test_result.IsTrailingHit()) {
        ++result;
    }
    return result;
}


void TextBox::OnDoubleClick(const DoubleClickInfo& event_info) {

    __super::OnDoubleClick(event_info);

    std::size_t click_index = FindIndexAtPosition(event_info.Position());
    SelectWordAtIndex(click_index);
}


void TextBox::SelectWordAtIndex(std::size_t index) {

    auto word_range = word_extractor_(GetText(), index);
    SetSelectionRange(word_range);
}


const text_box::WordExtractor& TextBox::WordExtractor() const noexcept {
    return word_extractor_;
}


void TextBox::SetWordExtractor(text_box::WordExtractor extractor) noexcept {
    if (extractor) {
        word_extractor_ = std::move(extractor);
    }
    else {
        word_extractor_ = text_box::DefaultWordExtractor();
    }
}


void TextBox::OnFocusGained(const FocusGainedInfo& event_info) {

    __super::OnFocusGained(event_info);

    UpdateCaretAtCurrentIndex();
    event_info.MarkAsHandled();

    if (selection_range_.length > 0) {
        NeedRepaint();
    }
}


void TextBox::OnFocusLost(const FocusLostInfo& event_info) {

    __super::OnFocusLost(event_info);

    caret_->SetIsVisible(false);
    event_info.MarkAsHandled();

    if (selection_range_.length > 0) {
        NeedRepaint();
    }
}


void TextBox::OnKeyDown(const KeyDownInfo& event_info) {

    __super::OnKeyDown(event_info);
    if (event_info.IsHandled()) {
        return;
    }

    HandleKeyDown(event_info);
    event_info.MarkAsHandled();
}


void TextBox::HandleKeyDown(const KeyDownInfo& event_info) {

    constexpr auto is_shift_pressed = []() {
        return !!(GetKeyState(VK_SHIFT) >> 15);
    };

    auto virtual_key = event_info.Message().VirtualKey();
    if (virtual_key == VK_LEFT) {
        BackwardCaretIndex(is_shift_pressed());
    }
    else if (virtual_key == VK_RIGHT) {
        ForwardCaretIndex(is_shift_pressed());
    }
    else if (virtual_key == VK_UP) {
        UpwardCaretIndex(is_shift_pressed());
    }
    else if (virtual_key == VK_DOWN) {
        DownwardCaretIndex(is_shift_pressed());
    }
    else if (virtual_key == L'C') {
        if (GetKeyState(VK_CONTROL) >> 15) {
            HandleCopy();
        }
    }
    else if (virtual_key == L'A') {
        if (GetKeyState(VK_CONTROL) >> 15) {
            HandleSelectAll();
        }
    }
}


void TextBox::BackwardCaretIndex(bool expand_selection) {

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

    SetCaretIndexByKey(new_index, expand_selection, true);
}


void TextBox::ForwardCaretIndex(bool expand_selection) {

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

    SetCaretIndexByKey(new_index, expand_selection, true);
}


void TextBox::UpwardCaretIndex(bool is_selecting_range) {
    UpdateCaretIndexVertically(false, is_selecting_range);
}


void TextBox::DownwardCaretIndex(bool is_selecting_range) {
    UpdateCaretIndexVertically(true, is_selecting_range);
}


void TextBox::UpdateCaretIndexVertically(bool is_downward, bool expand_selection) {

    auto caret_hit_test_result = GetTextLayout().HitTestIndex(caret_index_, false);

    auto current_line_info = LocateCurrentLineInfo();
    auto previous_line_y = caret_hit_test_result.Metrics().Top();
    if (is_downward) {
        previous_line_y += current_line_info.line_height;
    }
    else {
        previous_line_y -= current_line_info.line_height;
    }
    previous_line_y = (std::max)(previous_line_y, 0.f);

    auto previous_line_hit_test_result = GetTextLayout().HitTestPoint(
        Point{ caret_last_x_, previous_line_y });

    auto new_index = previous_line_hit_test_result.Metrics().TextIndex();
    if (previous_line_hit_test_result.IsTrailingHit()) {
        ++new_index;
    }

    SetCaretIndexByKey(new_index, expand_selection, false);
}


TextBox::LineInfo TextBox::LocateCurrentLineInfo() {

    auto line_metrics = GetTextLayout().GetLineMetrics();

    LineInfo line_info;

    for (const auto& each_line : line_metrics) {

        line_info.line_height = each_line.Height();

        auto line_end_index = line_info.line_char_index + each_line.Length();
        if (line_info.line_char_index <= caret_index_ && caret_index_ < line_end_index) {
            break;
        }

        line_info.line_y += each_line.Height();
        line_info.line_char_index = line_end_index;
    }

    return line_info;
}


void TextBox::OnTextChanged(const TextChangedInfo& event_info) {

    //Clear previous text rect and re-calulate it.
    text_rect_ = {};
    NeedRelayout();

    //Reset selection range on text changed. Note that this should be done before calling the same
    //method of base class.
    SetSelectionRange(Range{});

    __super::OnTextChanged(event_info);
}


std::size_t TextBox::CaretIndex() const {
    return caret_index_;
}


const Range& TextBox::SelectionRange() const {
    return selection_range_;
}


void TextBox::SetSelectionRange(const Range& range, bool scroll_to_selection) {

    auto text_length = core_->GetTextLength();
    auto begin_index = (std::min)(range.index, text_length);
    auto end_index = (std::min)(range.EndIndex(), text_length);
    selection_range_ = Range::FromIndexPair(begin_index, end_index);

    caret_index_ = selection_range_.EndIndex();
    AfterSetCaretIndex(true, scroll_to_selection);
}


std::wstring TextBox::SelectedText() const {

    auto text = GetText();
    auto selected_text = text.substr(selection_range_.index, selection_range_.length);
    return std::wstring{ selected_text };
}


void TextBox::SetCaretIndexByMouse(std::size_t new_index, bool begin_selection) {

    if (begin_selection) {
        begin_mouse_select_index_ = new_index;
        caret_index_ = new_index;
    }
    else {
        if (!begin_mouse_select_index_) {
            return;
        }
        caret_index_ = new_index;
    }

    selection_range_ = Range::FromIndexPair(
        (std::min)(*begin_mouse_select_index_, caret_index_),
        (std::max)(*begin_mouse_select_index_, caret_index_));

    AfterSetCaretIndex(true, true);
}


void TextBox::SetCaretIndexByKey(
    std::size_t new_index, 
    bool expand_selection,
    bool update_caret_x) {

    auto old_index = caret_index_;
    caret_index_ = (std::min)(new_index, core_->GetTextLength());

    std::size_t selection_begin = caret_index_;
    std::size_t selection_end = caret_index_;
    if (expand_selection) {

        if (old_index == selection_range_.Index()) {
            selection_begin = (std::min)(caret_index_, selection_range_.EndIndex());
            selection_end = (std::max)(caret_index_, selection_range_.EndIndex());
        }
        else if (old_index == selection_range_.EndIndex()) {
            selection_begin = (std::min)(selection_range_.Index(), caret_index_);
            selection_end = (std::max)(selection_range_.Index(), caret_index_);
        }
    }

    selection_range_ = Range::FromIndexPair(selection_begin, selection_end);
    AfterSetCaretIndex(update_caret_x, true);
}


void TextBox::AfterSetCaretIndex(bool update_caret_x, bool ensure_caret_visible) {

    auto hit_test_result = GetTextLayout().HitTestIndex(caret_index_, false);
    const auto& metrics = hit_test_result.Metrics();

    //If content size is empty, the cooridnate of metrics will be less than 0, which is abnormal.
    if (metrics.Left() < 0 || metrics.Top() < 0) {
        return;
    }

    if (ensure_caret_visible) {
        EnsureCaretVisible(metrics);
    }

    if (update_caret_x) {
        caret_last_x_ = metrics.Left();
    }

    ShowCaret(metrics);
    NeedRepaint();
}


void TextBox::UpdateCaretAtCurrentIndex() {

    auto hit_test_result = GetTextLayout().HitTestIndex(caret_index_, false);
    ShowCaret(hit_test_result.Metrics());
}


void TextBox::ShowCaret(const HitTestMetrics& metrics) {

    zaf::Rect caret_rect{ metrics.Left(), metrics.Top(), 1, metrics.Height() };
    caret_rect.AddOffset(text_rect_.position);

    caret_->SetRect(Align(caret_rect));

    if (this->IsFocused()) {
        caret_->SetIsVisible(true);
    }
}


void TextBox::EnsureCaretVisible(const HitTestMetrics& metrics) {

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
        metrics.Left(),
        metrics.Width(),
        text_rect_.position.x);

    //Update y.
    auto y_changed = update_single_dimension(
        content_size.height, 
        metrics.Top(),
        metrics.Height(), 
        text_rect_.position.y);

    if (x_changed) {
        SelfScrollingControlScrollValuesChangeInfo event_info;
        event_info.self_scrolling_control = this;
        event_info.is_horizontal = true;
        scroll_values_change_event_.Raise(event_info);
    }

    if (y_changed) {
        SelfScrollingControlScrollValuesChangeInfo event_info;
        event_info.self_scrolling_control = this;
        event_info.is_horizontal = false;
        scroll_values_change_event_.Raise(event_info);
    }

    if (x_changed || y_changed) {
        core_->LayoutText(text_rect_);
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


Observable<SelfScrollingControlScrollBarChangInfo> TextBox::ScrollBarChangeEvent() {
    return scroll_bar_change_event_.GetObservable();
}


Observable<SelfScrollingControlScrollValuesChangeInfo> TextBox::ScrollValuesChangeEvent() {
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

    core_->LayoutText(text_rect_);
    if (caret_->IsVisible()) {
        UpdateCaretAtCurrentIndex();
    }
    NeedRepaint();
}


std::size_t TextBox::LineCount() const {
    return GetTextLayout().GetMetrics().LineCount();
}


void TextBox::HandleCopy() {

    auto text = GetText();
    auto copied_text = text.substr(selection_range_.index, selection_range_.length);
    clipboard::Clipboard::SetText(copied_text);
}


void TextBox::HandleSelectAll() {
    SetSelectionRange(Range::Infinite());
}

}