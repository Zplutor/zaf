#include <zaf/control/control_binder.h>
#include <zaf/control/control.h>
#include <zaf/window/window.h>

namespace zaf::internal {

std::shared_ptr<Control> BridgedFindControl(const Control& control, const wchar_t* name) {
    return control.FindChild(name);
}


std::shared_ptr<Control> BridgedFindControl(const Window& window, const wchar_t* name) {
    return window.RootControl()->FindChild(name);
}

}