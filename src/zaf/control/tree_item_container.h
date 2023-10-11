#pragma once

#include <zaf/control/list_item_container.h>
#include <zaf/rx/subject.h>

namespace zaf {
namespace internal {
class TreeControlImplementation;
}

class TreeItemContainer : public ListItemContainer {
protected:
    void OnKeyDown(const KeyDownInfo& event_info) override;

private:
    friend class internal::TreeControlImplementation;

    Observable<bool> ChangeExpandStateEvent() {
        return change_expand_state_event_.AsObservable();
    }

private:
    Subject<bool> change_expand_state_event_;
};

}