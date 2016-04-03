#include <zaf/control/layout/anchor_layouter.h>
#include <zaf/control/control.h>

namespace zaf {

static void LayoutWithAnchor(
	const std::shared_ptr<Control>& parent,
	const Rect& previous_rect, 
	const std::vector<std::shared_ptr<Control>>& children
);

static void LayoutChild(
	const Rect& current_rect,
	const Rect& previous_rect, 
	const std::shared_ptr<Control>& child
);

const Layouter GetAnchorLayouter() {
	return LayoutWithAnchor;
}


static void LayoutWithAnchor(
	const std::shared_ptr<Control>& parent,
	const Rect& previous_rect,
	const std::vector<std::shared_ptr<Control>>& children
) {

	for (const auto& child : children) {
		LayoutChild(parent->GetRect(), previous_rect, child);
	}
}


static void LayoutChild(
	const Rect& current_rect,
	const Rect& previous_rect,
	const std::shared_ptr<Control>& child
) {

	auto change_single_dimension_with_anchor = [](
		bool has_front_anchor,
		bool has_back_anchor,
		float parent_old_size,
		float parent_new_size,
		float old_position,
		float old_size,
		float& new_position,
		float& new_size
	) {

		if (has_front_anchor && !has_back_anchor) {
			new_position = old_position;
			new_size = old_size;
		}
		else if (!has_front_anchor && has_back_anchor) {
			float old_back = parent_old_size - old_position - old_size;
			new_position = parent_new_size - old_back - old_size;
			new_size = old_size;
		}
		else if (has_front_anchor && has_back_anchor) {
			new_position = old_position;
			float old_back = parent_old_size - old_position - old_size;
			new_size = parent_new_size - old_back - new_position;
		}
		else {
			new_position = old_position;
			new_size = old_size;
		}
	};

	Anchor anchor = child->GetAnchor();
	bool has_left_anchor = (anchor & Anchor::Left) == Anchor::Left;
	bool has_right_anchor = (anchor & Anchor::Right) == Anchor::Right;
	bool has_top_anchor = (anchor & Anchor::Top) == Anchor::Top;
	bool has_bottom_anchor = (anchor & Anchor::Bottom) == Anchor::Bottom;

	const Rect& child_old_rect = child->GetRect();
	Rect child_new_rect;

	change_single_dimension_with_anchor(
		has_left_anchor,
		has_right_anchor,
		previous_rect.size.width,
		current_rect.size.width,
		child_old_rect.position.x,
		child_old_rect.size.width,
		child_new_rect.position.x,
		child_new_rect.size.width
	);

	change_single_dimension_with_anchor(
		has_top_anchor,
		has_bottom_anchor,
		previous_rect.size.height,
		current_rect.size.height,
		child_old_rect.position.y,
		child_old_rect.size.height,
		child_new_rect.position.y,
		child_new_rect.size.height
	);

	child->SetRect(child_new_rect);
}

}