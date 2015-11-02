#pragma once

namespace zaf {

class Control;
class Rect;

namespace internal {

class Layouter {
public:
	virtual ~Layouter() { }

	virtual void Layout(const Control& control, const Rect& previous_rect) = 0;
};

}
}