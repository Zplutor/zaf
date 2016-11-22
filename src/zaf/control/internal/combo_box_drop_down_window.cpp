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

    __super::WindowShow();
    container_->CaptureMouse();
}


void ComboBoxDropDownWindow::WindowDestroy(HWND handle) {

    __super::WindowDestroy(handle);
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


void ComboBoxDropDownWindow::Container::MouseCapture() {

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


void ComboBoxDropDownWindow::Container::MouseRelease() {
    originally_cursor_ = nullptr;
}


void ComboBoxDropDownWindow::Container::MouseMove(const Point& position, const MouseMessage& message) {

    if (originally_cursor_ != nullptr) {
        SetCursor(originally_cursor_);
    }
}


void ComboBoxDropDownWindow::Container::MouseUp(const Point& position, const MouseMessage& message) {

    if (!GetRect().Contain(position)) {
        ReleaseMouse();
    }
}


}
}