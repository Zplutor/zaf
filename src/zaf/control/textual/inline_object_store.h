#pragma once

#include <vector>
#include <zaf/base/event/event.h>
#include <zaf/base/non_copyable.h>
#include <zaf/base/range.h>
#include <zaf/control/textual/inline_object.h>

namespace zaf::textual {

/**
Stores and manages the attachment relations between inline objects and ranges.

This is a low level class used in styled text management.
*/
class InlineObjectStore : NonCopyable {
public:
    class Item {
    public:
        explicit Item(std::shared_ptr<InlineObject> object) : object_(std::move(object)) {

        }

        const Range& Range() const {
            return *object_->attached_range_;
        }

        const std::shared_ptr<InlineObject>& Object() const {
            return object_;
        }

    private:
        std::shared_ptr<InlineObject> object_;
    };

    using ItemList = std::vector<Item>;

public:
    InlineObjectStore() noexcept = default;
    ~InlineObjectStore();

    InlineObjectStore(InlineObjectStore&&) = default;
    InlineObjectStore& operator=(InlineObjectStore&&) = default;

    void Attach(std::shared_ptr<InlineObject> object, const Range& range);

    void ReplaceSpan(const Range& span_range, std::size_t new_length);

    void Clear();

    ItemList::const_iterator FindItemAtIndex(std::size_t index) const;
    ItemList::const_iterator FindFirstItemIntersectsWithRange(const Range& range) const;

    const ItemList& Items() const {
        return items_;
    }

    InlineObjectStore Clone() const;

private:
    ItemList items_;
};

}