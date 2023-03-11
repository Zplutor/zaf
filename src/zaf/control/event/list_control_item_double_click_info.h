#pragma once

#include <zaf/base/event/event_info.h>

namespace zaf {

class ListControl;

class ListControlItemDoubleClickInfo : public EventInfo {
public:
    explicit ListControlItemDoubleClickInfo(
        const std::shared_ptr<ListControl>& source,
        std::size_t item_index);

    std::size_t ItemIndex() const {
        return item_index_;
    }

private:
    std::size_t item_index_{};
};

}