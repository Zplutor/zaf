#pragma once

#include <zaf/control/clickable_control.h>

namespace zaf {

class Button : public ClickableControl {
public:
	Button();
	~Button();

	/**
	 Overrie. Change the default text alignment to TextAlignment::Center.
	 */
	TextAlignment GetDefaultTextAlignment() const {
		return TextAlignment::Center;
	}

	/**
	 Override. Change the default paragraph alignment to ParagraphAlignment::Center;
	 */
	ParagraphAlignment GetDefaultParagraphAlignment() const {
		return ParagraphAlignment::Center;
	}

protected:
	void Paint(Canvas& canvas, const Rect& dirty_rect) override;
};

}