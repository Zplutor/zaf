#include <zaf/control/internal/range_manager.h>
#include <set>

namespace zaf {
namespace internal {

void RangeManager::AddRange(const Range& new_range, std::any data) {

    //Ignore empty ranges.
    if (new_range.length == 0) {
        return;
    }

    //Loop over all items, to find insert position and modify existent ranges.
    auto iterator = items_.begin();
    while (iterator != items_.end()) {

        const auto current_range = iterator->range;

        //The beginning of the current range is greater than the end of the new range,
        //insert position is found.
        if (new_range.EndIndex() <= current_range.index) {
            break;
        }

        //The new range contains(or equals to) the current range, the current range should be
        //removed.
        if (new_range.Contain(current_range)) {
            iterator = items_.erase(iterator);
            continue;
        }

        //The new range is contained by the current range, the current range should be broken into
        //two sub ranges.
        if (current_range.Contain(new_range)) {

            //Modify current range to the first sub range.
            iterator->range.length = new_range.index - current_range.index;

            //Insert the latter sub range.
            Item latter_item;
            latter_item.range.index = new_range.EndIndex();
            latter_item.range.length = current_range.EndIndex() - new_range.EndIndex();
            latter_item.data = iterator->data;

            ++iterator;
            iterator = items_.insert(iterator, std::move(latter_item));
            break;
        }

        //The tail of the new range is overlapped with the head of the current range, the current
        //range should be modified.
        if ((new_range.index <= current_range.index) && 
            (new_range.EndIndex() > current_range.index)) {

            iterator->range.index = new_range.EndIndex();
            iterator->range.length = current_range.EndIndex() - new_range.EndIndex();
            break;
        }

        //The head of the new range is overlapped with the tail of the current range, the current
        //range should be modified.
        if ((new_range.index < current_range.EndIndex()) &&
            (new_range.EndIndex() >= current_range.EndIndex())) {

            iterator->range.length = new_range.index - current_range.index;
            ++iterator;
            continue;
        }

        ++iterator;
    }

    //Insert the new item to the position found.
    Item new_item;
    new_item.range = new_range;
    new_item.data = std::move(data);
    items_.insert(iterator, std::move(new_item));
}


bool RangeManager::AddRange(const Range& new_range) {
    return false;
}


void RangeManager::RemoveRange(const Range& removed_range) {

    if (removed_range.length == 0) {
        return;
    }

    auto iterator = ranges_.begin();
    while (iterator != ranges_.end()) {

        const auto current_range = *iterator;

        if (removed_range.EndIndex() <= current_range.index) {
            break;
        }

        if ((removed_range.index <= current_range.index) &&
            (removed_range.EndIndex() >= current_range.EndIndex())) {

            iterator = ranges_.erase(iterator);
            SendRangeNotify(RangeNotifyType::Remove, current_range.index, 0);
            continue;
        }

        if ((removed_range.index > current_range.index) &&
            (removed_range.EndIndex() < current_range.EndIndex())) {

            std::size_t breaked_range_position = removed_range.EndIndex();
            std::size_t breaked_range_length = current_range.EndIndex() - breaked_range_position;

            iterator->length = removed_range.index - current_range.index;
            ++iterator;
            iterator = ranges_.insert(
                iterator, 
                Range{ breaked_range_position, breaked_range_length });

            SendRangeNotify(
                RangeNotifyType::Break,
                current_range.index,
                breaked_range_position);
            continue;       
        }

        if ((removed_range.index <= current_range.index) &&
            (removed_range.EndIndex() > current_range.index)) {

            iterator->index = removed_range.EndIndex();
            iterator->length = current_range.EndIndex() - removed_range.EndIndex();
            SendRangeNotify(
                RangeNotifyType::Update,
                current_range.index,
                removed_range.EndIndex());

            ++iterator;
            continue;
        }

        if ((removed_range.index < current_range.EndIndex()) &&
            (removed_range.EndIndex() >= current_range.EndIndex())) {

            iterator->length = removed_range.index - current_range.index;
            ++iterator;
            continue;
        }

        ++iterator;
    }
}


void RangeManager::ExpandRanges(std::size_t position, std::size_t length) {

    if (length == 0) {
        return;
    }

    std::vector<std::tuple<RangeNotifyType, std::size_t, std::size_t>> notify_infos;

    auto iterator = ranges_.begin();
    while (iterator != ranges_.end()) {

        const auto& current_range = *iterator;

        if (position <= current_range.index) {

            iterator->index += length;
            notify_infos.push_back(std::make_tuple(
                RangeNotifyType::Update,
                current_range.index,
                iterator->index));
        }
        else if (position < current_range.EndIndex()) {

            std::size_t breaked_range_position = position + length;
            std::size_t breaked_range_length = current_range.EndIndex() - position;

            iterator->length = position - current_range.index;

            ++iterator;
            iterator = ranges_.insert(
                iterator, 
                Range{ breaked_range_position, breaked_range_length });

            notify_infos.push_back(std::make_tuple(
                RangeNotifyType::Break, 
                current_range.index, 
                breaked_range_position));
        }

        ++iterator;
    }

    for (const auto& each_info : notify_infos) {
        SendRangeNotify(std::get<0>(each_info), std::get<1>(each_info), std::get<2>(each_info));
    }
}


void RangeManager::NarrowRanges(std::size_t position, std::size_t length) {

    if (length == 0) {
        return;
    }

    std::vector<std::tuple<RangeNotifyType, std::size_t, std::size_t>> notify_infos;

    std::size_t end_position = position + length;

    auto iterator = ranges_.begin();
    while (iterator != ranges_.end()) {

        const auto current_range = *iterator;

        if (end_position <= current_range.index) {

            iterator->index -= length;
            notify_infos.push_back(std::make_tuple(
                RangeNotifyType::Update, 
                current_range.index, 
                iterator->index));
            ++iterator;
            continue;
        }

        if ((position <= current_range.index) &&
            (end_position >= current_range.EndIndex())) {

            iterator = ranges_.erase(iterator);
            notify_infos.push_back(std::make_tuple(
                RangeNotifyType::Remove, 
                current_range.index,
                0));
            continue;
        }

        if ((position > current_range.index) &&
            (end_position < current_range.EndIndex())) {

            iterator->length -= length;
            notify_infos.push_back(std::make_tuple(
                RangeNotifyType::Update,
                iterator->index, 
                iterator->index));

            ++iterator;
            continue;
        }

        if ((end_position > current_range.index) &&
            (end_position < current_range.EndIndex())) {

            iterator->index = position;
            iterator->length = current_range.EndIndex() - end_position;
            notify_infos.push_back(std::make_tuple(
                RangeNotifyType::Update, 
                current_range.index, 
                iterator->index));

            ++iterator;
            continue;
        }

        if ((position > current_range.index) &&
            (position < current_range.EndIndex())) {

            iterator->length -= current_range.EndIndex() - position;
            notify_infos.push_back(std::make_tuple(
                RangeNotifyType::Update,
                iterator->index,
                iterator->index));

            ++iterator;
            continue;
        }

        ++iterator;
    }

    for (const auto& each_info : notify_infos) {
        SendRangeNotify(std::get<0>(each_info), std::get<1>(each_info), std::get<2>(each_info));
    }
}


void RangeManager::RemoveAllRanges() {

    std::vector<std::size_t> removed_positions;

    for (const auto& each_range : ranges_) {
        removed_positions.push_back(each_range.index);
    }

    ranges_.clear();

    for (auto each_position : removed_positions) {
        SendRangeNotify(RangeNotifyType::Remove, each_position, 0);
    }
}


Range RangeManager::GetRangeContainsPosition(std::size_t position) const {

    for (const auto& each_range : ranges_) {
        if ((each_range.index <= position) && (position < each_range.index + each_range.length)) {
            return each_range;
        }
    }

    return Range{ 0, 0 };
}


}
}