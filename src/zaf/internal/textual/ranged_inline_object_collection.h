#pragma once

#include <vector>
#include <zaf/base/range.h>
#include <zaf/control/textual/inline_object.h>
#include <zaf/internal/textual/inline_object_wrapper.h>

namespace zaf::internal {

class RangedInlineObjectCollection {
public:
    class Item {
    public:
        Range range;
        InlineObjectWrapper object_wrapper;
    };

    using ItemList = std::vector<Item>;

public:
    std::vector<std::shared_ptr<textual::InlineObject>> Add(
        const Range& range,
        std::shared_ptr<textual::InlineObject> object);

    std::vector<std::shared_ptr<textual::InlineObject>> ReplaceSpan(
        const Range& span_range,
        std::size_t new_length);

    std::vector<std::shared_ptr<textual::InlineObject>> Clear();

    std::shared_ptr<textual::InlineObject> GetInlineObjectAtIndex(std::size_t index) const;

    const ItemList& Items() const {
        return items_;
    }

private:
    ItemList items_;
};

}