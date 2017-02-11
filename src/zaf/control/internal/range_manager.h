#pragma once

#include <functional>
#include <vector>
#include <zaf/base/define.h>

namespace zaf {
namespace internal {

class RangeManager {
public:
    enum class RangeNotifyType {
        Add,
        Remove,
        Update,
        Break,
    };

    typedef std::function<void(
        RangeNotifyType notify_type,
        std::size_t primary_position,
        std::size_t secondly_position)> RangeNotifyCallback;

public:
    RangeManager() { }

    RangeManager(const RangeNotifyCallback& notify_callback) : 
        notify_callback_(notify_callback) {

    }

    bool AddRange(std::size_t position, std::size_t length);
    void RemoveRange(std::size_t position, std::size_t length);
    
    void ExpandRanges(std::size_t position, std::size_t length);
    void NarrowRanges(std::size_t position, std::size_t length);

    void RemoveAllRanges();

    std::size_t GetRangeCount() const {
        return ranges_.size();
    }

    const std::pair<std::size_t, std::size_t> GetRangeAtIndex(std::size_t index) const {
        if (index < GetRangeCount()) {
            return ranges_[index];
        }
        else {
            return std::make_pair(InvalidIndex, 0);
        }
    }

    std::pair<std::size_t, std::size_t> GetRangeContainsPosition(std::size_t position) const;

    bool IsPositionInRange(std::size_t position) const {
        return GetRangeContainsPosition(position).first != InvalidIndex;
    }

    RangeManager(RangeManager&) = delete;
    RangeManager& operator=(RangeManager&) = delete;

private:
    void SendRangeNotify(
        RangeNotifyType notify_type, 
        std::size_t primary_position, 
        std::size_t secondly_position) {

        if (notify_callback_) {
            notify_callback_(notify_type, primary_position, secondly_position);
        }
    }

private:
    std::vector<std::pair<std::size_t, std::size_t>> ranges_;
    RangeNotifyCallback notify_callback_;
};

}
}