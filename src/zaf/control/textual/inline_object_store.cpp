#include <zaf/control/textual/inline_object_store.h>
#include <zaf/base/error/contract_error.h>

namespace zaf::textual {

InlineObjectStore::~InlineObjectStore() {

    //Detach each inline object so that they can be attached to another collection again.
    for (const auto& each_item : items_) {
        each_item.Object()->Detach();
    }
}


void InlineObjectStore::Attach(
    std::shared_ptr<InlineObject> object,
    const Range& range) {

    ZAF_EXPECT(object);
    ZAF_EXPECT(!object->attached_range_.has_value());

    if (range.IsEmpty()) {
        return;
    }

    std::vector<std::shared_ptr<textual::InlineObject>> detched_objects;

    auto iterator = items_.begin();
    while (iterator != items_.end()) {

        //Remove intersect ranges.
        if (iterator->Range().Intersects(range)) {

            detched_objects.push_back(iterator->Object());
            iterator = items_.erase(iterator);
            continue;
        }

        //Found insert position.
        if (iterator->Range().index >= range.EndIndex()) {
            break;
        }

        ++iterator;
    }

    object->attached_range_.emplace(range);
    items_.emplace(iterator, std::move(object));

    for (const auto& each_object : detched_objects) {
        each_object->Detach();
    }
}


void InlineObjectStore::ReplaceSpan(const Range& span_range, std::size_t new_length) {

    std::vector<std::shared_ptr<textual::InlineObject>> detached_objects;

    auto iterator = items_.begin();
    while (iterator != items_.end()) {

        //Modify the index of ranges which are after the span.
        if (iterator->Range().index >= span_range.EndIndex()) {

            std::size_t new_index = iterator->Range().index;
            new_index -= span_range.length;
            new_index += new_length;

            iterator->Object()->attached_range_->index = new_index;
            ++iterator;
            continue;
        }

        //Remove intersect ranges.
        if (iterator->Range().Contains(span_range) || iterator->Range().Intersects(span_range)) {

            detached_objects.push_back(iterator->Object());
            iterator = items_.erase(iterator);
            continue;
        }

        ++iterator;
    }

    for (const auto& each_object : detached_objects) {
        each_object->Detach();
    }
}


void InlineObjectStore::Clear() {

    auto items = std::move(items_);

    for (const auto& each_item : items) {
        each_item.Object()->Detach();
    }
}


InlineObjectStore::ItemList::const_iterator InlineObjectStore::FindItemAtIndex(
    std::size_t index) const {

    auto iterator = std::lower_bound(
        items_.begin(),
        items_.end(),
        index,
        [](const Item& item, std::size_t index) {
            return item.Range().EndIndex() <= index;
        });

    if (iterator != items_.end() && iterator->Range().Contains(index)) {
        return iterator;
    }
    return items_.end();
}


InlineObjectStore::ItemList::const_iterator InlineObjectStore::FindFirstItemContainedInRange(
    const Range& range) const noexcept {

    auto iterator = std::lower_bound(
        items_.begin(),
        items_.end(),
        range,
        [](const Item& item, const Range& range) {
            return item.Range().EndIndex() <= range.index;
        });

    for (; iterator != items_.end(); ++iterator) {
        if (range.Contains(iterator->Range())) {
            return iterator;
        }
    }

    return items_.end();
}


InlineObjectStore InlineObjectStore::Clone() const {

    InlineObjectStore result;
    for (const auto& each_item : items_) {

        const auto& old_object = each_item.Object();

        auto cloned_object = old_object->Clone();
        cloned_object->attached_range_ = old_object->attached_range_;

        result.items_.emplace_back(std::move(cloned_object));
    }
    return result;
}

}