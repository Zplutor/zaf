#include <zaf/control/internal/combo_box_drop_down_window.h>
#include <zaf/control/layout/linear_layouter.h>
#include <zaf/control/list_control.h>
#include <zaf/creation.h>

namespace zaf {
namespace internal {

void ComboBoxDropDownWindow::Initialize() {

    __super::Initialize();

    auto layouter = Create<VerticalLayouter>();

    container_ = Create<Container>();
    container_->SetLayouter(layouter);

    auto root_control = RootControl();
    root_control->SetLayouter(layouter);
    root_control->AddChild(container_);
}


void ComboBoxDropDownWindow::OnShow(const ShowInfo& event_info) {

    __super::OnShow(event_info);
    container_->CaptureMouse();
}


void ComboBoxDropDownWindow::OnMouseCaptureControlChanged(
    const MouseCaptureControlChangedInfo& event_info) {
    
    __super::OnMouseCaptureControlChanged(event_info);

    if (MouseCaptureControl() == nullptr) {

        if (event_info.PreviousControl() == container_) {
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


void ComboBoxDropDownWindow::Container::OnMouseCaptured(const MouseCapturedInfo& event_info) {

    __super::OnMouseCaptured(event_info);

    //Richedit calls SetCursor with NULL to hide the cursor when typing,
    //and the cursor would not be recovered when moving the mouse, because 
    //a capturing window doesn't receive WM_SETCURSOR. So we recover the 
    //cursor by ourself.

    CURSORINFO cursor_info = { 0 };
    cursor_info.cbSize = sizeof(cursor_info);
    BOOL is_succeeded = GetCursorInfo(&cursor_info);
    if (is_succeeded) {
        originally_cursor_ = cursor_info.hCursor;
    }
}


void ComboBoxDropDownWindow::Container::OnMouseReleased(const MouseReleasedInfo& event_info) {

    __super::OnMouseReleased(event_info);

    originally_cursor_ = nullptr;
}


void ComboBoxDropDownWindow::Container::OnMouseMove(const MouseMoveInfo& event_info) {

    __super::OnMouseMove(event_info);

    if (originally_cursor_ != nullptr) {
        SetCursor(originally_cursor_);
    }
}


void ComboBoxDropDownWindow::Container::OnMouseUp(const MouseUpInfo& event_info) {

    __super::OnMouseUp(event_info);

    if (!Rect().Contain(event_info.PositionAtSender())) {

        ReleaseMouse();
        event_info.MarkAsHandled();
    }
}


}
}