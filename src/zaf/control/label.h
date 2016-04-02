#pragma once

#include <zaf/control/control.h>

namespace zaf {

/**
 Represents a label control that displays static text.	
 */
class Label : public Control {
public:
	/**
	 Initialize the instance.
	 */
	Label();

	/**
	 Destroy the instance.
	 */
	~Label();

	void Initialize() override;

protected:
	void Paint(Canvas& canvas, const Rect& dirty_rect) override;
};

}