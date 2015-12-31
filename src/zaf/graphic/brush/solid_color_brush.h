#pragma once

#include <zaf/graphic/color.h>
#include <zaf/graphic/brush/brush.h>

namespace zaf {

class SolidColorBrush : public Brush {
public:
	explicit SolidColorBrush(ID2D1SolidColorBrush* handle) : 
		Brush(handle),
		handle_(handle) { }

	const Color GetColor() const {
		return Color::FromD2D1COLORF(handle_->GetColor());
	}

	void SetColor(const Color& color) {
		handle_->SetColor(color.ToD2D1COLORF());
	}

private:
	ID2D1SolidColorBrush* handle_;
};

}