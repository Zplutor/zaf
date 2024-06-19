#pragma once

#include <zaf/control/layout/axis_alignment.h>
#include <zaf/control/layout/layout_direction.h>
#include <zaf/control/layout/layouter.h>
#include <zaf/graphic/size.h>
#include <zaf/object/object.h>
#include <zaf/object/property_macros.h>

namespace zaf {
namespace internal {
class LinearLayoutLengthCalculatItem;
}


class LinearLayouter : public Layouter {
public:
    ZAF_DECLARE_TYPE;
    ZAF_DECLARE_PROPERTY;

public:
    LayoutDirection Direction() const {
        return direction_;
    }

    void SetDirection(LayoutDirection direction) {
        direction_ = direction;
    }

    zaf::AxisAlignment AxisAlignment() const {
        return axis_alignment_;
    }

    void SetAxisAlignment(zaf::AxisAlignment alignment) {
        axis_alignment_ = alignment;
    }

    zaf::AxisAlignment CrossAxisAlignment() const {
        return cross_axis_alignment_;
    }

    void SetCrossAxisAlignment(zaf::AxisAlignment alignment) {
        cross_axis_alignment_ = alignment;
    }

    void Layout(
        const Control& parent,
        const Rect& parent_old_rect,
        const std::vector<std::shared_ptr<Control>>& children) override;

private:
    std::vector<internal::LinearLayoutLengthCalculatItem> CalculateChildrenLengths(
        const std::vector<std::shared_ptr<Control>>& children,
        float content_length) const;

    std::vector<float> CalculateChildrenPositions(
        const std::vector<internal::LinearLayoutLengthCalculatItem>& length_items) const;

    float CalculatePositionOffset(
        const std::vector<float>& children_positions,
        float content_length) const;

    void SetRectsToChildren(
        const std::vector<float>& children_positions,
        float position_offset,
        const Size& content_size,
        const std::vector<std::shared_ptr<Control>>& children) const;

    Rect GetChildRect(
        const Control& child,
        float position, 
        float length, 
        const Size& content_size) const;

    bool IsVertical() const {
        return direction_ == LayoutDirection::TopToBottom;
    }

private:
    LayoutDirection direction_{ LayoutDirection::LeftToRight };
    zaf::AxisAlignment axis_alignment_{ AxisAlignment::Start };
    zaf::AxisAlignment cross_axis_alignment_{ AxisAlignment::Start };
};

ZAF_PROPERTY_BEGIN(LinearLayouter);
ZAF_PROPERTY(Direction);
ZAF_PROPERTY(AxisAlignment);
ZAF_PROPERTY(CrossAxisAlignment);
ZAF_PROPERTY_END;


class HorizontalLayouter : public LinearLayouter {
public:
    ZAF_DECLARE_TYPE

public:
    void Initialize() override;
};


class VerticalLayouter : public LinearLayouter {
public:
    ZAF_DECLARE_TYPE

public:
    void Initialize() override;
};


}