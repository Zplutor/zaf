#include <zaf/internal/textual/ranged_inline_object_collection.h>
#include <zaf/base/error/check.h>

namespace zaf::internal {

std::vector<std::shared_ptr<textual::InlineObject>> RangedInlineObjectCollection::Add(
    const Range& range,
    std::shared_ptr<textual::InlineObject> object) {

    ZAF_EXPECT(object);

    if (range.length == 0) {
        return {};
    }

    std::vector<std::shared_ptr<textual::InlineObject>> removed_objects;

    auto iterator = items_.begin();
    while (iterator != items_.end()) {

        //Remove intersect ranges.
        if (iterator->range.Intersects(range)) {
            removed_objects.push_back(iterator->object_wrapper.Detach());
            iterator = items_.erase(iterator);
            continue;
        }

        //Found insert position.
        if (iterator->range.index >= range.EndIndex()) {
            break;
        }

        ++iterator;
    }

    items_.emplace(iterator, range, InlineObjectWrapper{ std::move(object) });
    return removed_objects;
}


std::vector<std::shared_ptr<textual::InlineObject>> RangedInlineObjectCollection::ReplaceSpan(
    const Range& span_range,
    std::size_t new_length) {

    std::vector<std::shared_ptr<textual::InlineObject>> removed_objects;

    auto iterator = items_.begin();
    while (iterator != items_.end()) {

        //Remove intersect ranges.
        if (iterator->range.Intersects(span_range)) {
            removed_objects.push_back(iterator->object_wrapper.Detach());
            iterator = items_.erase(iterator);
            continue;
        }

        //Modify the index of ranges which are after the span.
        if (iterator->range.index >= span_range.EndIndex()) {
            iterator->range.index -= span_range.length;
            iterator->range.index += new_length;
        }

        ++iterator;
    }

    return removed_objects;
}


std::vector<std::shared_ptr<textual::InlineObject>> RangedInlineObjectCollection::Clear() {

    std::vector<std::shared_ptr<textual::InlineObject>> removed_objects;
    for (auto& each_item : items_) {
        removed_objects.push_back(each_item.object_wrapper.Detach());
    }

    items_.clear();
    return removed_objects;
}


std::shared_ptr<textual::InlineObject> RangedInlineObjectCollection::GetInlineObjectAtIndex(
    std::size_t index) const {

    for (const auto& each_item : items_) {
        if (each_item.range.Contains(index)) {
            return each_item.object_wrapper.Object();
        }
    }
    return nullptr;
}

}