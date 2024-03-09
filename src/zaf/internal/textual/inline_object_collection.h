#pragma once

#include <vector>
#include <zaf/base/event/event.h>
#include <zaf/base/non_copyable.h>
#include <zaf/base/range.h>
#include <zaf/control/textual/inline_object.h>
#include <zaf/internal/textual/inline_object_changed_info.h>

namespace zaf::internal {

class InlineObjectCollection : NonCopyable {
public:
    class Item {
    public:
        Item(const Range& range, std::shared_ptr<textual::InlineObject> object) :
            range_(range),
            object_(std::move(object)) {

        }

        const Range& Range() const {
            return range_;
        }

        const std::shared_ptr<textual::InlineObject>& Object() const {
            return object_;
        }

    private:
        zaf::Range range_;
        std::shared_ptr<textual::InlineObject> object_;
    };

    using ItemList = std::vector<Item>;

public:
    void Add(const Range& range, std::shared_ptr<textual::InlineObject> object);

    void ReplaceSpan(const Range& span_range, std::size_t new_length);

    void Clear();

    std::shared_ptr<textual::InlineObject> GetInlineObjectAtIndex(std::size_t index) const;

    const ItemList& Items() const {
        return items_;
    }

    Observable<InlineObjectChangedInfo> ChangedEvent() const {
        return changed_event_.GetObservable();
    }

private:
    ItemList items_;
    Event<InlineObjectChangedInfo> changed_event_;
};

}