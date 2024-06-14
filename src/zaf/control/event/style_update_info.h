#pragma once

#include <zaf/base/event/event_info.h>

namespace zaf {

class Control;

class StyleUpdateInfo : public EventInfo {
public:
    explicit StyleUpdateInfo(const std::shared_ptr<Control>& control);
};

}