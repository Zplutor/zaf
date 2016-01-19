#include <zaf/internal/anchor_layouter.h>
#include <zaf/control/control.h>
#include <zaf/base/rect.h>

namespace zaf {
namespace internal {

static void LayoutChild(const std::shared_ptr<Control>& child, const Rect& current_rect, const Rect& previous_rect);
static void ChangeSingleDimensionWithAnchor(
	bool has_front_anchor,
	bool has_back_anchor,
	float parent_old_position,
	float parent_old_size,
	float parent_new_position,
	float parent_new_size,
	float old_position,
	float old_size,
	float& new_position,
	float& new_size
);

void AnchorLayouter::Layout(const Control& control, const Rect& previous_rect) {

	for (auto& child : control.GetChildren()) {
		LayoutChild(child, control.GetRect(), previous_rect);
	}
}


static void LayoutChild(const std::shared_ptr<Control>& child, const Rect& current_rect, const Rect& previous_rect) {

	const auto& anchors = child->GetAnchors();

	bool has_left_anchor = anchors.find(Control::Anchor::Left) != anchors.end();
	bool has_top_anchor = anchors.find(Control::Anchor::Top) != anchors.end();
	bool has_right_anchor = anchors.find(Control::Anchor::Right) != anchors.end();
	bool has_bottom_anchor = anchors.find(Control::Anchor::Bottom) != anchors.end();

	const Rect& child_old_rect = child->GetRect();
	Rect child_new_rect;

	ChangeSingleDimensionWithAnchor(
		has_left_anchor,
		has_right_anchor,
		previous_rect.position.x,
		previous_rect.size.width,
		current_rect.position.x,
		current_rect.size.width,
		child_old_rect.position.x,
		child_old_rect.size.width,
		child_new_rect.position.x,
		child_new_rect.size.width
	);

	ChangeSingleDimensionWithAnchor(
		has_top_anchor,
		has_bottom_anchor,
		previous_rect.position.y,
		previous_rect.size.height,
		current_rect.position.x,
		current_rect.size.height,
		child_old_rect.position.y,
		child_old_rect.size.height,
		child_new_rect.position.y,
		child_new_rect.size.height
	);

	child->SetRect(child_new_rect);
}


static void ChangeSingleDimensionWithAnchor(
	bool has_front_anchor,
	bool has_back_anchor,
	float parent_old_position,
	float parent_old_size,
	float parent_new_position,
	float parent_new_size,
	float old_position,
	float old_size,
	float& new_position,
	float& new_size
) {

	if (has_front_anchor && ! has_back_anchor) {
		new_position = old_position;
		new_size = old_size;
	}
	else if (! has_front_anchor && has_back_anchor) {
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

		if (parent_old_size != 0) {
			new_position = (old_position / parent_old_size) * parent_new_size;
			new_size = (old_size / parent_old_size) * parent_new_size;
		}
	}
}


}
}