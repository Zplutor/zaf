#include <zaf/control/linear_box.h>
#include <zaf/control/control_object.h>

namespace zaf {

ZAF_OBJECT_IMPL(LinearBox)


LinearBox::LinearBox() : layouter_(Create<LinearLayouter>()) {

}


LinearBox::LinearBox(const std::shared_ptr<LinearLayouter>& layouter) : layouter_(layouter) {

}


void LinearBox::Initialize() {

    __super::Initialize();

    SetLayouter(layouter_);
}


ZAF_OBJECT_IMPL(VerticalBox);

VerticalBox::VerticalBox() : LinearBox(Create<VerticalLayouter>()) {

}


ZAF_OBJECT_IMPL(HorizontalBox);

HorizontalBox::HorizontalBox() : LinearBox(Create<HorizontalLayouter>()) {

}

}