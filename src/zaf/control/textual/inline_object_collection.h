#pragma once

#include <vector>
#include <zaf/base/event/event.h>
#include <zaf/base/non_copyable.h>
#include <zaf/base/range.h>
#include <zaf/control/textual/inline_object.h>

namespace zaf::textual {

/**
A collection that manages the attachment relations between inline objects and ranges.

This is a low level class used in styled text management.
*/
class InlineObjectCollection : NonCopyable {
public:
    class Item {
    public:
        Item(std::shared_ptr<InlineObject> object) : object_(std::move(object)) {

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
    InlineObjectCollection() noexcept = default;
    ~InlineObjectCollection();

    void Attach(std::shared_ptr<InlineObject> object, const Range& range);

    void ReplaceSpan(const Range& span_range, std::size_t new_length);

    void Clear();

    std::shared_ptr<InlineObject> GetInlineObjectAtIndex(std::size_t index) const;

    const ItemList& Items() const {
        return items_;
    }

private:
    ItemList items_;
};

}