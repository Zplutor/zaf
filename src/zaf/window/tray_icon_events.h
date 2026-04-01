#pragma once

#include <zaf/base/event/event_info.h>
#include <zaf/graphic/point.h>

namespace zaf {

class TrayIcon;

class TrayIconActivateInfo : public EventInfo {
public:
    explicit TrayIconActivateInfo(std::shared_ptr<TrayIcon> source);
};


class TrayIconContextMenuInfo : public EventInfo {
public:
    TrayIconContextMenuInfo(std::shared_ptr<TrayIcon> source, const Point& position_in_global);

    const Point& PositionInGlobal() const noexcept {
        return position_in_global_;
    }

private:
    Point position_in_global_{};
};

}