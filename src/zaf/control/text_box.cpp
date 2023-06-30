#include <zaf/control/text_box.h>
#include <zaf/base/log.h>
#include <zaf/graphic/canvas.h>
#include <zaf/object/type_definition.h>

namespace zaf {

ZAF_DEFINE_TYPE(TextBox)
ZAF_DEFINE_TYPE_END

void TextBox::Paint(Canvas& canvas, const zaf::Rect& dirty_rect) {

    __super::Paint(canvas, dirty_rect);

    PaintSelectionBackground(canvas, dirty_rect);
}


void TextBox::PaintSelectionBackground(Canvas& canvas, const zaf::Rect& dirty_rect) {

    auto text_layout = GetTextLayout();

    auto min = (std::min)(begin_select_index_, end_select_index_);
    auto max = (std::max)(begin_select_index_, end_select_index_);

    UINT32 metrics_count{};
    HRESULT hresult = text_layout.Inner()->HitTestTextRange(
        static_cast<UINT32>(min),
        static_cast<UINT32>(max - min),
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
        static_cast<UINT32>(min),
        static_cast<UINT32>(max - min),
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


void TextBox::OnMouseMove(const MouseMoveInfo& event_info) {

    __super::OnMouseMove(event_info);

    if (!is_selecting_) {
        return;
    }

    auto text_layout = GetTextLayout();

    BOOL is_tailing{};
    BOOL is_inside{};
    DWRITE_HIT_TEST_METRICS metrics{};
    HRESULT hresult = text_layout.Inner()->HitTestPoint(
        event_info.PositionAtSource().x,
        event_info.PositionAtSource().y,
        &is_tailing,
        &is_inside,
        &metrics);

    if (FAILED(hresult)) {
        return;
    }

    end_select_index_ = metrics.textPosition;

    NeedRepaint();
}


void TextBox::OnMouseDown(const MouseDownInfo& event_info) {

    __super::OnMouseDown(event_info);

    auto text_layout = GetTextLayout();

    BOOL is_tailing{};
    BOOL is_inside{};
    DWRITE_HIT_TEST_METRICS metrics{};
    HRESULT hresult = text_layout.Inner()->HitTestPoint(
        event_info.PositionAtSource().x,
        event_info.PositionAtSource().y,
        &is_tailing,
        &is_inside,
        &metrics);

    if (FAILED(hresult)) {
        return;
    }

    begin_select_index_ = metrics.textPosition;
    is_selecting_ = true;
}


void TextBox::OnMouseUp(const MouseUpInfo& event_info) {

    __super::OnMouseUp(event_info);

    if (!is_selecting_) {
        return;
    }

    auto text_layout = GetTextLayout();

    BOOL is_tailing{};
    BOOL is_inside{};
    DWRITE_HIT_TEST_METRICS metrics{};
    HRESULT hresult = text_layout.Inner()->HitTestPoint(
        event_info.PositionAtSource().x,
        event_info.PositionAtSource().y,
        &is_tailing,
        &is_inside,
        &metrics);

    if (FAILED(hresult)) {
        return;
    }

    end_select_index_ = metrics.textPosition;
    is_selecting_ = false;

    NeedRepaint();
}

}