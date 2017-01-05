#include <zaf/control/internal/combo_box_drop_down_window.h>
#include <zaf/control/layout/array_layouter.h>
#include <zaf/control/list_control.h>
#include <zaf/creation.h>

namespace zaf {
namespace internal {

void ComboBoxDropDownWindow::Initialize() {

    __super::Initialize();

    auto root_control = GetRootControl();
    root_control->SetLayouter(GetVerticalArrayLayouter());
}


void ComboBoxDropDownWindow::SetListControl(const std::shared_ptr<ListControl>& list_control) {

    auto root_control = GetRootControl();

    if (list_control_ != nullptr) {
        root_control->RemoveChild(list_control_);
    }

    list_control_ = list_control;
    root_control->AddChild(list_control_);
}


}
}