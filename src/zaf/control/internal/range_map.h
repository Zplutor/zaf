#pragma once

#include <map>
#include <zaf/base/container/utility/find.h>
#include <zaf/control/internal/range_manager.h>

namespace zaf {
namespace internal {

template<typename ValueType>
class RangeMap {
public:
    RangeMap() : 
        range_manager_(
            std::bind(
                &RangeMap::RangeChange,
                this, 
                std::placeholders::_1, 
                std::placeholders::_2,
                std::placeholders::_3)) {
    
    }

    void AddValueToRange(const ValueType& value, std::size_t position, std::size_t length) {

        if (range_manager_.AddRange(position, length)) {
            values_.insert(std::make_pair(position, value));
        }
    }

    void RemoveAllValues() {
        range_manager_.RemoveAllRanges();
    }

    ValueType GetValueAtPosition(std::size_t position, bool* is_existent = nullptr) const {

        if (is_existent) {
            *is_existent = false;
        }

        auto range = range_manager_.GetRangeContainsPosition(position);
        if (range.length == 0) {
            return {};
        }

        auto value = Find(values_, range.index);
        if (!value) {
            return {};
        }

        if (is_existent) {
            *is_existent = true;
        }
        return *value;
    }

    std::vector<std::pair<Range, ValueType>> GetAllRangesAndValues() const {

        std::size_t range_count = range_manager_.GetRangeCount();

        std::vector<std::pair<Range, ValueType>> ranges_and_values;
        ranges_and_values.reserve(range_count);

        for (std::size_t index = 0; index < range_count; ++index) {

            auto range = range_manager_.GetRangeAtIndex(index);

            ValueType value;
            auto value_iterator = values_.find(range.index);
            if (value_iterator != values_.end()) {
                value = value_iterator->second;
            }
            
            ranges_and_values.push_back(std::make_pair(range, value));
        }

        return ranges_and_values;
    }

    RangeMap(RangeMap&) = delete;
    RangeMap& operator=(RangeMap&) = delete;

private:
    void RangeChange(
        RangeManager::RangeNotifyType notify_type, 
        std::size_t primary_position, 
        std::size_t secondly_position) {

        switch (notify_type) {

            case RangeManager::RangeNotifyType::Remove:
                values_.erase(primary_position);
                break;

            case RangeManager::RangeNotifyType::Update: {
                auto value = values_[primary_position];
                values_[secondly_position] = value;
                values_.erase(primary_position);
                break;
            }

            case RangeManager::RangeNotifyType::Break: {
                auto value = values_[primary_position];
                values_[secondly_position] = value;
                break;
            }

            default:
                break;
        }
    }

private:
    RangeManager range_manager_;
    std::map<std::size_t, ValueType> values_;
};

}
}