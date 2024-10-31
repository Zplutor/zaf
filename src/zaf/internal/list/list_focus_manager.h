#pragma once

#include <zaf/control/list_item.h>
#include <zaf/control/event/focus_event_info.h>
#include <zaf/internal/list/list_control_parts_based.h>
#include <zaf/internal/list/list_focus_store.h>

namespace zaf::internal {

class ListFocusManager: public ListControlPartsBased {
public:
    using ListControlPartsBased::ListControlPartsBased;

    std::optional<std::size_t> FocusedIndex() const noexcept;

    void ChangeFocusByUserInput(std::optional<std::size_t> focused_index);

    void ChangeFocusByOwnerFocusGainedEvent(const FocusGainedInfo& event_info);
    void ChangeFocusByOwnerFocusLostEvent(const FocusLostInfo& event_info);

    void ChangeFocusByAddingData(const Range& added_range);
    void ChangeFocusByRemovingData(const Range& removed_range);
    void ChangeFocusByMovingData(std::size_t from, std::size_t to);

    void ChangeFocusBeforeRemovingVisibleItem(const ListItem& item);
    void ChangeFocusAfterAddingVisibleItem(ListItem& item, std::size_t index);

    void ChangeFocusByReloading();

private:
    void SetFocusToProperControlIfCan() const;
    void SetFocusToProperControl(std::optional<std::size_t> focused_index) const;

private:
    /**
    Records the index of the focused item.

    This index is mainly used for recovering the focus of the item which is scrolled back to the 
    visible area from invisible area.
    */
    ListFocusStore store_;

    mutable bool is_setting_focus_to_control_{};
    bool is_removing_visible_item_{};
};

}