#include <zaf/internal/textual/inline_object_collection.h>
#include <zaf/base/error/check.h>

namespace zaf::internal {

void InlineObjectCollection::Add(
    const Range& range, 
    std::shared_ptr<textual::InlineObject> object) {

    ZAF_EXPECT(object);

    if (range.length == 0) {
        return;
    }

    std::vector<std::shared_ptr<textual::InlineObject>> removed_objects;

    auto iterator = items_.begin();
    while (iterator != items_.end()) {

        //Remove intersect ranges.
        if (iterator->Range().Intersects(range)) {
            removed_objects.push_back(iterator->Object());
            iterator = items_.erase(iterator);
            continue;
        }

        //Found insert position.
        if (iterator->Range().index >= range.EndIndex()) {
            break;
        }

        ++iterator;
    }

    items_.emplace(iterator, range, object);

    std::vector<std::shared_ptr<textual::InlineObject>> added_objects{ std::move(object) };

    changed_event_.Raise(InlineObjectChangedInfo{ 
        std::move(added_objects), 
        std::move(removed_objects) 
    });
}


void InlineObjectCollection::ReplaceSpan(const Range& span_range, std::size_t new_length) {

    std::vector<std::shared_ptr<textual::InlineObject>> removed_objects;

    auto iterator = items_.begin();
    while (iterator != items_.end()) {

        //Modify the index of ranges which are after the span.
        if (iterator->Range().index >= span_range.EndIndex()) {

            std::size_t new_index = iterator->Range().index;
            new_index -= span_range.length;
            new_index += new_length;

            *iterator = Item{ Range{ new_index, iterator->Range().length }, iterator->Object() };
            ++iterator;
            continue;
        }

        //Remove intersect ranges.
        if (iterator->Range().Contains(span_range) || iterator->Range().Intersects(span_range)) {
            removed_objects.push_back(iterator->Object());
            iterator = items_.erase(iterator);
            continue;
        }

        ++iterator;
    }

    changed_event_.Raise(InlineObjectChangedInfo{ {}, std::move(removed_objects) });
}


void InlineObjectCollection::Clear() {

    std::vector<std::shared_ptr<textual::InlineObject>> removed_objects;
    for (auto& each_item : items_) {
        removed_objects.push_back(each_item.Object());
    }

    items_.clear();
    changed_event_.Raise(InlineObjectChangedInfo{ {}, std::move(removed_objects) });
}


std::shared_ptr<textual::InlineObject> InlineObjectCollection::GetInlineObjectAtIndex(
    std::size_t index) const {

    for (const auto& each_item : items_) {
        if (each_item.Range().Contains(index)) {
            return each_item.Object();
        }
    }
    return nullptr;
}

}