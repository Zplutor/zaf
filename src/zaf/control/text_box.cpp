#include <zaf/control/text_box.h>
#include <zaf/base/as.h>
#include <zaf/base/log.h>
#include <zaf/control/internal/textual_control/text_box_core.h>
#include <zaf/graphic/canvas.h>
#include <zaf/object/type_definition.h>

namespace zaf {

ZAF_DEFINE_TYPE(TextBox)
ZAF_DEFINE_TYPE_END

TextBox::TextBox() : TextualControl(std::make_unique<internal::TextBoxCore>()) {

}


void TextBox::PaintSelectionBackground(Canvas& canvas, const zaf::Rect& dirty_rect) {

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

    for (UINT32 index = 0; index < metrics_count; ++index) {

        const auto& metrics = buffer.get()[index];
        
        zaf::Rect rect{ metrics.left, metrics.top, metrics.width, metrics.height };
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

    auto index = FindTextIndexAtPoint(event_info.PositionAtSource());
    if (!index) {
        return;
    }

    CaptureMouse();

    selecting_indexes_ = std::make_pair(*index, *index);
    DetermineSelectionRange();
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

    if (!selecting_indexes_) {
        return;
    }

    auto index = FindTextIndexAtPoint(event_info.PositionAtSource());
    if (!index) {
        return;
    }

    selecting_indexes_->second = *index;
    DetermineSelectionRange();
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

    if (!selecting_indexes_) {
        return;
    }

    ReleaseMouse();

    auto index = FindTextIndexAtPoint(event_info.PositionAtSource());
    if (index) {

        selecting_indexes_->second = *index;
        DetermineSelectionRange();
    }

    selecting_indexes_.reset();
    NeedRepaint();
}


std::optional<std::size_t> TextBox::FindTextIndexAtPoint(const Point& point) const {

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

    if (is_tailing) {
        return metrics.textPosition + 1;
    }

    return metrics.textPosition;
}


void TextBox::DetermineSelectionRange() {

    if (!selecting_indexes_) {
        return;
    }

    selection_range_ = Range::FromIndexPair(
        (std::min)(selecting_indexes_->first, selecting_indexes_->second),
        (std::max)(selecting_indexes_->first, selecting_indexes_->second));
}


TextLayout TextBox::GetTextLayout() const {
    return As<internal::TextBoxCore>(Core())->GetTextLayout();
}

}