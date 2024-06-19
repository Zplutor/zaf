#pragma once

#include <zaf/control/control.h>
#include <zaf/control/layout/axis_alignment.h>
#include <zaf/control/layout/layout_direction.h>
#include <zaf/control/layout/linear_layouter.h>
#include <zaf/object/property_macros.h>

namespace zaf {

class LinearBox : public Control {
public:
    ZAF_DECLARE_TYPE;
    ZAF_DECLARE_PROPERTY;

    LinearBox();

    LayoutDirection Direction() const {
        return layouter_->Direction();
    }

    void SetDirection(LayoutDirection direction) {
        layouter_->SetDirection(direction);
        NeedRelayout();
    }

    zaf::AxisAlignment AxisAlignment() const {
        return layouter_->AxisAlignment();
    }

    void SetAxisAlignment(zaf::AxisAlignment alignment) {
        layouter_->SetAxisAlignment(alignment);
        NeedRelayout();
    }

    zaf::AxisAlignment CrossAxisAlignment() const {
        return layouter_->CrossAxisAlignment();
    }

    void SetCrossAxisAlignment(zaf::AxisAlignment alignment) {
        layouter_->SetCrossAxisAlignment(alignment);
        NeedRelayout();
    }

protected:
    LinearBox(const std::shared_ptr<LinearLayouter>& layouter);

    void Initialize() override;

private:
    std::shared_ptr<LinearLayouter> layouter_;
};

ZAF_PROPERTY_BEGIN(LinearBox);
ZAF_PROPERTY(Direction)
ZAF_PROPERTY(AxisAlignment)
ZAF_PROPERTY(CrossAxisAlignment)
ZAF_PROPERTY_END;


class VerticalBox : public LinearBox {
public:
    ZAF_DECLARE_TYPE;

    VerticalBox();
};


class HorizontalBox : public LinearBox {
public:
    ZAF_DECLARE_TYPE;

    HorizontalBox();
};

}