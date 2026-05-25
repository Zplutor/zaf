#include <zaf/graphic/canvas/canvas_state.h>
#include <zaf/graphic/canvas/canvas.h>

namespace zaf {

CanvasState::CanvasState(
    Canvas* canvas,
    internal::CanvasStateData* data,
    std::size_t tag) noexcept
    : 
    canvas_(canvas),
    data_(data),
    tag_(tag) {

}


CanvasState::~CanvasState() {
    PopState();
}


CanvasState::CanvasState(CanvasState&& other) noexcept {

    canvas_ = other.canvas_;
    data_ = other.data_;
    tag_ = other.tag_;

    other.canvas_ = nullptr;
    other.data_ = nullptr;
    other.tag_ = 0;
}


void CanvasState::SetPixelSnapMode(PixelSnapMode pixel_snap_mode) {
    ZAF_EXPECT(data_);
    data_->pixel_snap_mode = pixel_snap_mode;
}


void CanvasState::SetBrush(d2d::Brush brush) {
    ZAF_EXPECT(data_);
    data_->brush = std::move(brush);
}


void CanvasState::SetBrush(const Color& color) {
    ZAF_EXPECT(canvas_);
    auto brush = canvas_->Renderer().CreateSolidColorBrush(color);
    this->SetBrush(std::move(brush));
}


void CanvasState::SetStrokeWidth(float stroke_width) {
    ZAF_EXPECT(data_);
    data_->stroke_width = stroke_width;
}


void CanvasState::SetStrokeStyle(d2d::StrokeStyle stroke_style) {
    ZAF_EXPECT(data_);
    data_->stroke_style = std::move(stroke_style);
}


void CanvasState::PopState() {
    if (canvas_) {
        canvas_->PopState(tag_);
    }
}

}