#pragma once

namespace zaf {
namespace internal {

template<int>
class ListDataEventInfo {
public:
    ListDataEventInfo(std::size_t index, std::size_t count) : index_(index), count_(count) {

    }

    std::size_t Index() const {
        return index_;
    }

    std::size_t Count() const {
        return count_;
    }

private:
    std::size_t index_{};
    std::size_t count_{};
};

}

using ListDataAddedInfo = internal::ListDataEventInfo<__LINE__>;
using ListDataRemovedInfo = internal::ListDataEventInfo<__LINE__>;
using ListDataUpdatedInfo = internal::ListDataEventInfo<__LINE__>;


class ListDataMovedInfo {
public:
    ListDataMovedInfo(std::size_t previous_index, std::size_t new_index) : 
        previous_index_(previous_index), 
        new_index_(new_index) {

    }

    std::size_t PreviousIndex() const {
        return previous_index_;
    }

    std::size_t NewIndex() const {
        return new_index_;
    }

private:
    std::size_t previous_index_{};
    std::size_t new_index_{};
};

}