#pragma once

#include <zaf/graphic/color.h>
#include <zaf/graphic/d2d/brush.h>

namespace zaf::d2d {

/**
 Represents a solid color brush.

 You can create instances via Renderer::CreateSolidColorBrush.
 */
class SolidColorBrush : public Brush {
public:
    SolidColorBrush() = default;

    explicit SolidColorBrush(COMPtr<ID2D1SolidColorBrush> ptr) : 
        Brush(ptr),
        ptr_(std::move(ptr)) { }

    /**
     Get color of brush.
     */
    const Color GetColor() const {
        return Color::FromD2D1COLORF(Ptr()->GetColor());
    }

    /**
     Set color of brush.
     */
    void SetColor(const Color& color) {
        Ptr()->SetColor(color.ToD2D1COLORF());
    }

    const COMPtr<ID2D1SolidColorBrush>& Ptr() const noexcept {
        return ptr_;
    }

private:
    COMPtr<ID2D1SolidColorBrush> ptr_;
};

}