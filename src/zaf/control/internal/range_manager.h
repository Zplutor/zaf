#pragma once

#include <any>
#include <functional>
#include <vector>
#include <zaf/base/non_copyable.h>
#include <zaf/base/range.h>

namespace zaf {
namespace internal {

class RangeManager : NonCopyableNonMovable {
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

    void AddRange(const Range& new_range, std::any data);

    bool AddRange(const Range& new_range);
    bool AddRange(std::size_t position, std::size_t length) {
        return AddRange(Range{ position, length });
    }

    void RemoveRange(const Range& removed_range);
    void RemoveRange(std::size_t position, std::size_t length) {
        RemoveRange(Range{ position, length });
    }
    
    void ExpandRanges(std::size_t position, std::size_t length);
    void NarrowRanges(std::size_t position, std::size_t length);

    void RemoveAllRanges();

    std::size_t GetRangeCount() const {
        return ranges_.size();
    }

    Range GetRangeAtIndex(std::size_t index) const {
        if (index < ranges_.size()) {
            return ranges_[index];
        }
        else {
            return Range{ 0, 0 };
        }
    }

    Range GetRangeContainsPosition(std::size_t position) const;

    bool IsPositionInRange(std::size_t position) const {
        return GetRangeContainsPosition(position).length != 0;
    }

private:
    class Item {
    public:
        Range range;
        std::any data;
    };

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
    std::vector<Item> items_;
    std::vector<Range> ranges_;
    RangeNotifyCallback notify_callback_;
};

}
}