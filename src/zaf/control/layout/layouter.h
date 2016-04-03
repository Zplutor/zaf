#pragma once

#include <functional>
#include <memory>
#include <vector>

namespace zaf {

class Control;
class Rect;

typedef std::function<
	void(
		const std::shared_ptr<Control>& parent,
		const Rect& preivous_rect,
		const std::vector<std::shared_ptr<Control>>& children
	)
> Layouter;

}