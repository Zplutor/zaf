#include <zaf/control/linear_box.h>
#include <zaf/control/layout/linear_layouter.h>
#include <zaf/object/type_definition.h>

namespace zaf {

ZAF_DEFINE_TYPE(VerticalBox)
ZAF_DEFINE_END

void VerticalBox::Initialize() {

    __super::Initialize();

    SetLayouter(Create<VerticalLayouter>());
}


ZAF_DEFINE_TYPE(HorizontalBox)
ZAF_DEFINE_END

void HorizontalBox::Initialize() {

    __super::Initialize();

    SetLayouter(Create<HorizontalLayouter>());
}

}