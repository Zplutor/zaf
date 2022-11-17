#include <zaf/control/property_grid/value_view.h>
#include <zaf/control/layout/linear_layouter.h>
#include <zaf/creation.h>

namespace zaf::property_grid {

void ValueView::Initialize() {

    __super::Initialize();

    this->SetLayouter(Create<HorizontalLayouter>());
}

}