#include <zaf/window/tray_icon_events.h>
#include <zaf/window/tray_icon.h>

namespace zaf {

TrayIconActivateInfo::TrayIconActivateInfo(std::shared_ptr<TrayIcon> source) :
    EventInfo(std::move(source)) {

}


TrayIconContextMenuInfo::TrayIconContextMenuInfo(
    std::shared_ptr<TrayIcon> source,
    const Point& position_in_global)
    :
    EventInfo(std::move(source)),
    position_in_global_(position_in_global){

}

}