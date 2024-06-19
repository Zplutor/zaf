#include <zaf/control/linear_box.h>
#include <zaf/object/type_definition.h>

namespace zaf {

ZAF_DEFINE_TYPE(LinearBox)
ZAF_DEFINE_TYPE_END

ZAF_DEFINE_PROPERTY(LinearBox);


LinearBox::LinearBox() : layouter_(Create<LinearLayouter>()) {

}


LinearBox::LinearBox(const std::shared_ptr<LinearLayouter>& layouter) : layouter_(layouter) {

}


void LinearBox::Initialize() {

    __super::Initialize();

    SetLayouter(layouter_);
}


ZAF_DEFINE_TYPE(VerticalBox)
ZAF_DEFINE_TYPE_END

VerticalBox::VerticalBox() : LinearBox(Create<VerticalLayouter>()) {

}


ZAF_DEFINE_TYPE(HorizontalBox)
ZAF_DEFINE_TYPE_END

HorizontalBox::HorizontalBox() : LinearBox(Create<HorizontalLayouter>()) {

}

}