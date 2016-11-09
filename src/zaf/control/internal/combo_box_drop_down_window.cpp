#include <zaf/control/internal/combo_box_drop_down_window.h>
#include <zaf/control/layout/array_layouter.h>
#include <zaf/control/list_control.h>
#include <zaf/creation.h>

namespace zaf {
namespace internal {

void ComboBoxDropDownWindow::Initialize() {

    __super::Initialize();

    container_ = Create<Container>();
    container_->SetLayouter(GetVerticalArrayLayouter());

    auto root_control = GetRootControl();
    root_control->SetLayouter(GetVerticalArrayLayouter());
    root_control->AddChild(container_);
}


void ComboBoxDropDownWindow::WindowShow() {
    container_->CaptureMouse();
}


void ComboBoxDropDownWindow::CapturingMouseControlChange(const std::shared_ptr<Control>& previous_control) {
    
    if (GetCapturingMouseControl() == nullptr) {

        if (previous_control == container_) {
            Close();
        }
        else {
            container_->CaptureMouse();
        }
    }
}


void ComboBoxDropDownWindow::SetListControl(const std::shared_ptr<ListControl>& list_control) {

    if (list_control_ != nullptr) {
        container_->RemoveChild(list_control_);
    }

    list_control_ = list_control;
    container_->AddChild(list_control_);
}


void ComboBoxDropDownWindow::Container::MouseUp(const Point& position, const MouseMessage& message) {

    if (!GetRect().Contain(position)) {
        ReleaseMouse();
    }
}

}
}