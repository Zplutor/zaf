#pragma once

#include <zaf/control/list_item_container.h>
#include <zaf/rx/subject/subject.h>

namespace zaf {
namespace internal {
class TreeCore;
}

class TreeItemContainer : public ListItemContainer {
protected:
    void OnKeyDown(const KeyDownInfo& event_info) override;

private:
    friend class internal::TreeCore;

    rx::Observable<bool> ChangeExpandStateEvent() {
        return change_expand_state_event_.AsObservable();
    }

private:
    rx::Subject<bool> change_expand_state_event_;
};

}