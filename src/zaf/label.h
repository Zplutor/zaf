#pragma once

#include <zaf/control/control.h>

namespace zaf {

class Label : public Control {
public:
	const std::wstring& GetText() const {
		return text_;
	}

	void SetText(const std::wstring& text) {
		text_ = text;
		NeedRepaint();
	}

protected:
	void Paint(Canvas& canvas, const Rect& dirty_rect) override;

private:
	std::wstring text_;
};

}