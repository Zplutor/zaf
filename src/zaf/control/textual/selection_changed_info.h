#pragma once

#include <zaf/base/event/event_info.h>

namespace zaf {

class TextBox;

namespace textual {

class SelectionChangedInfo : public EventInfo {
public:
    explicit SelectionChangedInfo(std::shared_ptr<TextBox> source);
};

}
}