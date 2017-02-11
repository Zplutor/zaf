#include <zaf/control/internal/range_manager.h>
#include <set>

namespace zaf {
namespace internal {

bool RangeManager::AddRange(std::size_t position, std::size_t length) {

    if ((position == InvalidIndex) || (length == 0)) {
        return false;
    }

    std::size_t new_range_begin_position = position;
    std::size_t new_range_end_position = position + length;

    auto iterator = ranges_.begin();
    while (iterator != ranges_.end()) {

        std::size_t current_range_begin_position = iterator->first;
        std::size_t current_range_end_position = iterator->first + iterator->second;

        if (new_range_end_position <= current_range_begin_position) {
            break;
        }

        if ((new_range_begin_position <= current_range_begin_position) &&
            (new_range_end_position >= current_range_end_position)) {

            iterator = ranges_.erase(iterator);
            SendRangeNotify(RangeNotifyType::Remove, current_range_begin_position, InvalidIndex);
            continue;
        }

        if ((new_range_begin_position > current_range_begin_position) &&
            (new_range_end_position < current_range_end_position)) {

            iterator->second = new_range_begin_position - iterator->first;

            std::size_t breaked_range_position = new_range_end_position;
            std::size_t breaked_range_length = current_range_end_position - breaked_range_position;

            ++iterator;
            iterator = ranges_.insert(iterator, std::make_pair(breaked_range_position, breaked_range_length));

            SendRangeNotify(RangeNotifyType::Break, current_range_begin_position, breaked_range_position);
            break;
        }

        if ((new_range_begin_position <= current_range_begin_position) &&
            (new_range_end_position > current_range_begin_position)) {

            iterator->first = new_range_end_position;
            iterator->second = current_range_end_position - iterator->first;
            SendRangeNotify(RangeNotifyType::Update, current_range_begin_position, iterator->first);
            break;
        }

        if ((new_range_begin_position < current_range_end_position) &&
            (new_range_end_position >= current_range_end_position )) {

            iterator->second = new_range_begin_position - iterator->first;
            ++iterator;
            continue;
        }

        ++iterator;
    }

    iterator = ranges_.insert(iterator, std::make_pair(position, length));
    SendRangeNotify(RangeNotifyType::Add, iterator->first, InvalidIndex);
    return true;
}


void RangeManager::RemoveRange(std::size_t position, std::size_t length) {

    if ((position == InvalidIndex) || (length == 0)) {
        return;
    }

    std::size_t removed_range_begin_position = position;
    std::size_t removed_range_end_position = position + length;

    auto iterator = ranges_.begin();
    while (iterator != ranges_.end()) {

        std::size_t current_range_begin_position = iterator->first;
        std::size_t current_range_end_position = iterator->first + iterator->second;

        if (removed_range_end_position <= current_range_begin_position) {
            break;
        }

        if ((removed_range_begin_position <= current_range_begin_position) &&
            (removed_range_end_position >= current_range_end_position)) {

            iterator = ranges_.erase(iterator);
            SendRangeNotify(RangeNotifyType::Remove, current_range_begin_position, InvalidIndex);
            continue;
        }

        if ((removed_range_begin_position > current_range_begin_position) &&
            (removed_range_end_position < current_range_end_position)) {

            std::size_t breaked_range_position = removed_range_end_position;
            std::size_t breaked_range_length = current_range_end_position - breaked_range_position;

            iterator->second = removed_range_begin_position - iterator->first;
            ++iterator;
            iterator = ranges_.insert(iterator, std::make_pair(breaked_range_position, breaked_range_length));

            SendRangeNotify(RangeNotifyType::Break, current_range_begin_position, breaked_range_position);
            continue;       
        }

        if ((removed_range_begin_position <= current_range_begin_position) &&
            (removed_range_end_position > current_range_begin_position)) {

            iterator->first = removed_range_end_position;
            iterator->second = current_range_end_position - iterator->first;
            SendRangeNotify(RangeNotifyType::Update, current_range_begin_position, iterator->first);

            ++iterator;
            continue;
        }

        if ((removed_range_begin_position < current_range_end_position) &&
            (removed_range_end_position >= current_range_end_position)) {

            iterator->second = removed_range_begin_position - iterator->first;
            ++iterator;
            continue;
        }

        ++iterator;
    }
}


void RangeManager::ExpandRanges(std::size_t position, std::size_t length) {

    if ((position == InvalidIndex) || (length == 0)) {
        return;
    }

    std::vector<std::tuple<RangeNotifyType, std::size_t, std::size_t>> notify_infos;

    auto iterator = ranges_.begin();
    while (iterator != ranges_.end()) {

        std::size_t current_range_begin_position = iterator->first;
        std::size_t current_range_end_position = iterator->first + iterator->second;

        if (position <= current_range_begin_position) {

            iterator->first += length;
            notify_infos.push_back(std::make_tuple(RangeNotifyType::Update, current_range_begin_position, iterator->first));
        }
        else if (position < current_range_end_position) {

            std::size_t breaked_range_position = position + length;
            std::size_t breaked_range_length = current_range_end_position - position;

            iterator->second = position - current_range_begin_position;

            ++iterator;
            iterator = ranges_.insert(iterator, std::make_pair(breaked_range_position, breaked_range_length));
            notify_infos.push_back(std::make_tuple(RangeNotifyType::Break, current_range_begin_position, breaked_range_position));
        }

        ++iterator;
    }

    for (const auto& each_info : notify_infos) {
        SendRangeNotify(std::get<0>(each_info), std::get<1>(each_info), std::get<2>(each_info));
    }
}


void RangeManager::NarrowRanges(std::size_t position, std::size_t length) {

    if ((position == InvalidIndex) || (length == 0)) {
        return;
    }

    std::vector<std::tuple<RangeNotifyType, std::size_t, std::size_t>> notify_infos;

    std::size_t end_position = position + length;

    auto iterator = ranges_.begin();
    while (iterator != ranges_.end()) {

        std::size_t current_range_begin_position = iterator->first;
        std::size_t current_range_end_position = iterator->first + iterator->second;

        if (end_position <= current_range_begin_position) {

            iterator->first -= length;
            notify_infos.push_back(std::make_tuple(RangeNotifyType::Update, current_range_begin_position, iterator->first));
            ++iterator;
            continue;
        }

        if ((position <= current_range_begin_position) && 
            (end_position >= current_range_end_position)) {

            iterator = ranges_.erase(iterator);
            notify_infos.push_back(std::make_tuple(RangeNotifyType::Remove, current_range_begin_position, InvalidIndex));
            continue;
        }

        if ((position > current_range_begin_position) && 
            (end_position < current_range_end_position)) {

            iterator->second -= length;
            notify_infos.push_back(std::make_tuple(RangeNotifyType::Update, iterator->first, iterator->first));

            ++iterator;
            continue;
        }

        if ((end_position > current_range_begin_position) &&
            (end_position < current_range_end_position)) {

            iterator->first = position;
            iterator->second = current_range_end_position - end_position;
            notify_infos.push_back(std::make_tuple(RangeNotifyType::Update, current_range_begin_position, iterator->first));

            ++iterator;
            continue;
        }

        if ((position > current_range_begin_position) &&
            (position < current_range_end_position)) {

            iterator->second -= current_range_end_position - position;
            notify_infos.push_back(std::make_tuple(RangeNotifyType::Update, iterator->first, iterator->first));

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
        removed_positions.push_back(each_range.first);
    }

    ranges_.clear();

    for (auto each_position : removed_positions) {
        SendRangeNotify(RangeNotifyType::Remove, each_position, InvalidIndex);
    }
}


std::pair<std::size_t, std::size_t> RangeManager::GetRangeContainsPosition(std::size_t position) const {

    for (const auto& each_range : ranges_) {
        if ((each_range.first <= position) && (position < each_range.first + each_range.second)) {
            return each_range;
        }
    }

    return std::make_pair(InvalidIndex, 0);
}


}
}