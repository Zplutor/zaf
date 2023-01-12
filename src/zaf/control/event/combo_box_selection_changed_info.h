#pragma once

#include <zaf/base/event/event_info.h>

namespace zaf {

class ComboBox;

class ComboBoxSelectionChangedInfo : public EventInfo {
public:
    explicit ComboBoxSelectionChangedInfo(const std::shared_ptr<ComboBox>& source);
};

}