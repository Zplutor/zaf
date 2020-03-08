#pragma once

#include <zaf/control/layout/layouter.h>

namespace zaf {

class LinearLayouter : public Layouter {
public:
	enum class Direction {
		LeftToRight,
		TopToBottom,
	};

public:
	void SetDirection(Direction direction) {
		direction_ = direction;
	}

	void Layout(
		const Control& parent,
		const Rect& parent_old_rect,
		const std::vector<std::shared_ptr<Control>>& children
	) override;

private:
	Direction direction_{ Direction::LeftToRight };
};

}