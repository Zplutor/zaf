#include <zaf/internal/textual/inline_object_collection.h>
#include <zaf/base/error/check.h>

namespace zaf::internal {

void InlineObjectCollection::Add(
    const Range& range, 
    std::shared_ptr<textual::InlineObject> object) {

    ZAF_EXPECT(object);
    ZAF_EXPECT(!object->attach_info_.has_value());

    if (range.length == 0) {
        return;
    }

    std::vector<std::shared_ptr<textual::InlineObject>> removed_objects;

    auto iterator = items_.begin();
    while (iterator != items_.end()) {

        //Remove intersect ranges.
        if (iterator->Range().Intersects(range)) {

            const auto& removed_object = iterator->Object();
            removed_object->attach_info_.reset();

            removed_objects.push_back(removed_object);
            iterator = items_.erase(iterator);
            continue;
        }

        //Found insert position.
        if (iterator->Range().index >= range.EndIndex()) {
            break;
        }

        ++iterator;
    }

    object->attach_info_.emplace(range);
    items_.emplace(iterator, object);

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

            iterator->Object()->attach_info_->range.index = new_index;
            ++iterator;
            continue;
        }

        //Remove intersect ranges.
        if (iterator->Range().Contains(span_range) || iterator->Range().Intersects(span_range)) {

            const auto& removed_object = iterator->Object();
            removed_object->attach_info_.reset();

            removed_objects.push_back(removed_object);
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

        const auto& object = each_item.Object();
        object->attach_info_.reset();
        removed_objects.push_back(object);
    }

    items_.clear();
    changed_event_.Raise(InlineObjectChangedInfo{ {}, std::move(removed_objects) });
}


std::shared_ptr<textual::InlineObject> InlineObjectCollection::GetInlineObjectAtIndex(
    std::size_t index) const {

    auto iterator = std::lower_bound(
        items_.begin(), 
        items_.end(), 
        index,
        [](const Item& item, std::size_t index) {
            return item.Range().EndIndex() <= index;
        });

    if (iterator != items_.end() && iterator->Range().Contains(index)) {
        return iterator->Object();
    }
    return nullptr;
}

}