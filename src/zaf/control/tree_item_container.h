#pragma once

#include <zaf/control/list_item_container.h>
#include <zaf/rx/subject.h>

namespace zaf {
namespace internal {
class TreeControlImplementation;
}

class TreeItemContainer : public ListItemContainer {
protected:
    bool OnKeyDown(const KeyMessage& message) override;

private:
    friend class internal::TreeControlImplementation;

    Observable<bool> ChangeExpandStateEvent() {
        return change_expand_state_event_.GetObservable();
    }

private:
    Subject<bool> change_expand_state_event_;
};

}