#pragma once

#include <zaf/graphic/color.h>
#include <zaf/graphic/brush/brush.h>

namespace zaf {

/**
 Represents a solid color brush.

 You can create instances via Renderer::CreateSolidColorBrush.
 */
class SolidColorBrush : public Brush {
public:
    SolidColorBrush() = default;
    explicit SolidColorBrush(COMPtr<ID2D1SolidColorBrush> inner) : 
        Brush(inner),
        inner_(std::move(inner)) { }

    /**
     Get color of brush.
     */
    const Color GetColor() const {
        return Color::FromD2D1COLORF(Inner()->GetColor());
    }

    /**
     Set color of brush.
     */
    void SetColor(const Color& color) {
        Inner()->SetColor(color.ToD2D1COLORF());
    }

    const COMPtr<ID2D1SolidColorBrush>& Inner() const noexcept {
        return inner_;
    }

private:
    COMPtr<ID2D1SolidColorBrush> inner_;
};

}