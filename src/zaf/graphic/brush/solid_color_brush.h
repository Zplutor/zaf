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
    /**
     Construct the instance with specified handle.

     See also Brush::Brush.
     */
	explicit SolidColorBrush(ID2D1SolidColorBrush* handle) : 
		Brush(handle),
		handle_(handle) { }

    /**
     Get color of brush.
     */
	const Color GetColor() const {
		return Color::FromD2D1COLORF(handle_->GetColor());
	}

    /**
     Set color of brush.
     */
	void SetColor(const Color& color) {
		handle_->SetColor(color.ToD2D1COLORF());
	}

private:
	ID2D1SolidColorBrush* handle_;
};

}