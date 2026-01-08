#include <zaf/window/event/dpi_changed_info.h>
#include <zaf/window/window.h>

namespace zaf {

DPIChangedInfo::DPIChangedInfo(const std::shared_ptr<Window>& window) : EventInfo(window) {

}

}
