#include <zaf/control/control_binder.h>
#include <zaf/control/control.h>
#include <zaf/window/window.h>

namespace zaf::internal {

std::shared_ptr<Control> BridgedFindControl(const Control& control, const wchar_t* name) {
    return control.FindChild(name);
}


std::shared_ptr<Control> BridgedFindControl(const Window& window, const wchar_t* name) {

    const auto& root_control = window.RootControl();
    if (root_control->Name() == name) {
        return root_control;
    }
    return root_control->FindChild(name);
}

}