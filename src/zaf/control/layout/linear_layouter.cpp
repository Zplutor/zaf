#include <zaf/control/layout/linear_layouter.h>
#include <zaf/control/control.h>
#include <zaf/control/layout/internal/linear_layout_length_calculating.h>
#include <zaf/reflection/reflection_type_definition.h>
#include <zaf/parsing/parsers/linear_layouter_parser.h>

namespace zaf {
namespace {

template<bool IsHeight>
class DimensionTraits;

template<>
class DimensionTraits<false> {
public:
	static float GetLength(const Size& size) {
		return size.width;
	}

	static float GetMinimumLength(const Control& control) {
		return control.GetMinimumWidth();
	}

	static float GetMaximumLength(const Control& control) {
		return control.GetMaximumWidth();
	}

	static float GetHeadingMargin(const Frame& frame) {
		return frame.left;
	}

	static float GetTailingMargin(const Frame& frame) {
		return frame.right;
	}
};

template<>
class DimensionTraits<true> {
public:
	static float GetLength(const Size& size) {
		return size.height;
	}

	static float GetMinimumLength(const Control& control) {
		return control.GetMinimumHeight();
	}

	static float GetMaximumLength(const Control& control) {
		return control.GetMaximumHeight();
	}

	static float GetHeadingMargin(const Frame& frame) {
		return frame.top;
	}

	static float GetTailingMargin(const Frame& frame) {
		return frame.bottom;
	}
};


float CalculateAxisOffset(
	float length,
	float avaliable_length,
	AxisAlignment axis_alignment) {

	if (axis_alignment == AxisAlignment::Near) {
		return 0;
	}

	float space_length = avaliable_length - length;

	if (axis_alignment == AxisAlignment::Far) {
		return space_length;
	}

	if (axis_alignment == AxisAlignment::Center) {
		return space_length / 2;
	}

	return 0;
}


template<bool IsHeightDimension>
class ControlRectCalculator {
public:
	static Rect Calculate(
		const Control& control,
		float position,
		float length,
		const Size& content_size,
		AxisAlignment axis_alignment) {

		auto current_dimension_info = CalculateCurrentDimensionPositionAndLength(
			control, 
			position, 
			length);

		auto other_dimension_info = CalculateOtherDimensionPositionAndLength(
			control,
			content_size, 
			axis_alignment);

		if (IsHeightDimension) {
			std::swap(current_dimension_info, other_dimension_info);
		}

		Rect rect;
		rect.position.x = current_dimension_info.first;
		rect.position.y = other_dimension_info.first;
		rect.size.width = current_dimension_info.second;
		rect.size.height = other_dimension_info.second;
		return rect;
	}

private:
	static std::pair<float, float> CalculateCurrentDimensionPositionAndLength(
		const Control& control,
		float position,
		float length) {

		using Dimension = DimensionTraits<IsHeightDimension>;

		const auto& margin = control.GetMargin();

		float actual_position = position + Dimension::GetHeadingMargin(margin);

		float actual_length =
			length -
			Dimension::GetHeadingMargin(margin) -
			Dimension::GetTailingMargin(margin);

		return std::make_pair(actual_position, actual_length);
	}


	static std::pair<float, float> CalculateOtherDimensionPositionAndLength(
		const Control& control,
		const Size& content_size,
		AxisAlignment axis_alignment) {

		using Dimension = DimensionTraits<!IsHeightDimension>;

		const auto& margin = control.GetMargin();

		//Calculate length.
		float avaliable_length =
			Dimension::GetLength(content_size) -
			Dimension::GetHeadingMargin(margin) -
			Dimension::GetTailingMargin(margin);

		float length = (std::min)(avaliable_length, Dimension::GetMaximumLength(control));
		length = (std::max)(length, Dimension::GetMinimumLength(control));

		//Calculate position.
		float position = 
			Dimension::GetHeadingMargin(margin) +
			CalculateAxisOffset(length, avaliable_length, axis_alignment);

		return std::make_pair(position, length);
	}
};

}


std::shared_ptr<LinearLayouter> LinearLayouter::CreateLeftToRightLayouter() {
	auto layouter = zaf::Create<LinearLayouter>();
	layouter->SetDirection(zaf::LayoutDirection::LeftToRight);
	return layouter;
}


std::shared_ptr<LinearLayouter> LinearLayouter::CreateTopToBottomLayouter() {
	auto layouter = zaf::Create<LinearLayouter>();
	layouter->SetDirection(zaf::LayoutDirection::TopToBottom);
	return layouter;
}


void LinearLayouter::Layout(
	const Control& parent,
	const Rect& parent_old_rect,
	const std::vector<std::shared_ptr<Control>>& children) {

	if (children.empty()) {
		return;
	}

	auto content_size = parent.GetContentSize();
	float content_length = IsVertical() ? content_size.height : content_size.width;

	//Calcuate length for each child.
	auto length_items = CalculateChildrenLengths(children, content_length);

	//Calculate position for each child according their lengths.
	auto children_positions = CalculateChildrenPositions(length_items);

	//Calculate position offset of the first child.
	float position_offset = CalculatePositionOffset(children_positions, content_length);

	//Set position and size to each child.
	SetRectsToChildren(children_positions, position_offset, content_size, children);
}


std::vector<internal::LinearLayoutLengthCalculatItem> LinearLayouter::CalculateChildrenLengths(
	const std::vector<std::shared_ptr<Control>>& children,
	float content_length) const {

	std::vector<internal::LinearLayoutLengthCalculatItem> result;
	result.reserve(children.size());

	for (const auto& each_child : children) {

		internal::LinearLayoutLengthCalculatItem item;

		item.minimum_length =
			IsVertical() ? each_child->GetMinimumHeight() : each_child->GetMinimumWidth();

		item.maximum_length =
			IsVertical() ? each_child->GetMaximumHeight() : each_child->GetMaximumWidth();

		//Add margin length.
		const auto& margin = each_child->GetMargin();
		float margin_length = 
			IsVertical() ? margin.top + margin.bottom : margin.left + margin.right;

		item.minimum_length += margin_length;
		item.maximum_length += margin_length;

		result.push_back(item);
	}

	internal::CalculateArrayLayoutLengths(content_length, result);
	return result;
}


std::vector<float> LinearLayouter::CalculateChildrenPositions(
	const std::vector<internal::LinearLayoutLengthCalculatItem>& length_items) const {

	std::vector<float> result;
	result.reserve(length_items.size() + 1);

	float current_position = 0;
	for (const auto& each_item : length_items) {

		result.push_back(current_position);
		current_position += each_item.current_length;
	}

	result.push_back(current_position);
	return result;
}


float LinearLayouter::CalculatePositionOffset(
	const std::vector<float>& children_positions,
	float content_length) const {

	if (control_alignment_ == ControlAlignment::Leading) {
		return 0;
	}
	
	float total_length = children_positions.back() - children_positions.front();
	float space_length = content_length - total_length;

	if (control_alignment_ == ControlAlignment::Tailing) {
		return space_length;
	}

	if (control_alignment_ == ControlAlignment::Center) {
		return space_length / 2;
	}

	return 0;
}


ZAF_DEFINE_REFLECTION_TYPE(LinearLayouter, ReflectionObject);


void LinearLayouter::SetRectsToChildren(
	const std::vector<float>& children_positions,
	float position_offset,
	const Size& content_size,
	const std::vector<std::shared_ptr<Control>>& children) const {

	float actual_position = position_offset;

	for (std::size_t index = 0; index < children.size(); ++index) {

		float actual_length = children_positions[index + 1] - children_positions[index];

		Rect child_rect = GetChildRect(
			*children[index], 
			actual_position, 
			actual_length, 
			content_size);

		children[index]->SetRect(child_rect);

		actual_position += actual_length;
	}
}


Rect LinearLayouter::GetChildRect(
	const Control& child,
	float position,
	float length,
	const Size& content_size) const {

	if (IsVertical()) {
		return ControlRectCalculator<true>::Calculate(
			child,
			position,
			length, 
			content_size, 
			axis_alignment_);
	}
	else {
		return ControlRectCalculator<false>::Calculate(
			child,
			position,
			length,
			content_size,
			axis_alignment_);
	}
}


float LinearLayouter::CalculateAxisOffset(
	float control_length,
	float content_length) const {

	if (axis_alignment_ == AxisAlignment::Near) {
		return 0;
	}

	float space_length = content_length - control_length;

	if (axis_alignment_ == AxisAlignment::Far) {
		return space_length;
	}

	if (axis_alignment_ == AxisAlignment::Center) {
		return space_length / 2;
	}

	return 0;
}

}