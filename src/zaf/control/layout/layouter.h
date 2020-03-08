#pragma once

#include <functional>
#include <memory>
#include <vector>

namespace zaf {

class Control;
class Rect;

class Layouter {
public:
	Layouter() = default;
	virtual ~Layouter() = default;

	Layouter(const Layouter&) = delete;
	Layouter& operator=(const Layouter&) = delete;

	virtual void Layout(
		const Control& parent,
		const Rect& parent_old_rect,
		const std::vector<std::shared_ptr<Control>>& children
	) = 0;
};


using LayoutFunction = std::function<void(
	const Control & parent,
	const Rect & parent_old_rect,
	const std::vector<std::shared_ptr<Control>> & children
)>;

std::shared_ptr<Layouter> CreateLayouter(const LayoutFunction& layout_function);

}