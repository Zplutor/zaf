#pragma once

#include <deque>
#include <zaf/base/non_copyable.h>
#include <zaf/control/list_control_delegate.h>
#include <zaf/control/list_data_source.h>
#include <zaf/control/list_item.h>
#include <zaf/control/list_item_container.h>
#include <zaf/control/scroll_bar.h>
#include <zaf/control/scroll_box.h>
#include <zaf/control/selection_mode.h>
#include <zaf/internal/list/list_control_parts_based.h>
#include <zaf/internal/list/list_focus_store.h>
#include <zaf/internal/list/list_selection_store.h>
#include <zaf/rx/subscription_host.h>
#include <zaf/window/popup_menu.h>

namespace zaf::internal {

class ListControlCore : public ListControlPartsBased, SubscriptionHost {
public:
    using DataSourceChangeEvent = std::function<void(const std::shared_ptr<ListDataSource>&)>;
    using DelegateChangeEvent = std::function<void(const std::shared_ptr<ListControlDelegate>&)>;
    using ItemContainerChangeEvent = 
        std::function<void(const std::shared_ptr<ListItemContainer>&)>;
    using SelectionChangedEvent = std::function<void()>;
    using ItemDoubleClickEvent = std::function<void(std::size_t)>;
    using ContextMenuEvent = std::function<
        std::shared_ptr<PopupMenu>(
            std::optional<std::size_t> item_index, 
            const std::shared_ptr<Object>& item_data)
    >;

    class InitializeParameters {
    public:
        std::weak_ptr<ListDataSource> data_source;
        std::weak_ptr<ListControlDelegate> delegate;
        std::shared_ptr<ListItemContainer> item_container;
        DataSourceChangeEvent data_source_change_event;
        DelegateChangeEvent delegate_change_event;
        ItemContainerChangeEvent item_container_change_event;
        SelectionChangedEvent selection_changed_event;
        ItemDoubleClickEvent item_double_click_event;
        ContextMenuEvent context_menu_event;
    };

public:
    using ListControlPartsBased::ListControlPartsBased;

    void Initialize(const InitializeParameters& parameters);

    std::shared_ptr<ListDataSource> DataSource() const noexcept;
    void SetDataSource(const std::weak_ptr<ListDataSource>& data_source);

    std::shared_ptr<ListControlDelegate> Delegate() const noexcept;
    void SetDelegate(const std::weak_ptr<ListControlDelegate>& delegate);

    const std::shared_ptr<ListItemContainer>& ItemContainer() const noexcept;
    void SetItemContainer(const std::shared_ptr<ListItemContainer>& item_container);

    void OnLayout();
    void OnVerticalScrollBarChange();

    void HandleFocusGainedEvent(const FocusGainedInfo& event_info);
    void HandleFocusLostEvent(const FocusLostInfo& event_info);

    void Reload();

    void ScrollToItemAtIndex(std::size_t index);

    std::optional<std::size_t> FindItemIndexAtPosition(const Point& position);

    bool AutoAdjustScrollBarSmallChange() const {
        return auto_adjust_scroll_bar_small_change_;
    }

    void SetAutoAdjustScrollBarSmallChange(bool value) {
        auto_adjust_scroll_bar_small_change_ = value;
        if (auto_adjust_scroll_bar_small_change_) {
            AdjustScrollBarSmallChange();
        }
    }

private:
    void InstallDataSource(const std::weak_ptr<ListDataSource>& data_source);
    void InstallDelegate(const std::weak_ptr<ListControlDelegate>& delegate);
    void InstallItemContainer(const std::shared_ptr<ListItemContainer>& item_container);
    void OnItemContainerDoubleClick(const DoubleClickInfo& event_info);
    void OnItemContainerMouseUp(const MouseUpInfo& event_info);

    void RegisterScrollBarEvents();
    void UnregisterScrollBarEvents();
    void AdjustScrollBarSmallChange();

    void RegisterDataSourceEvents();

    void RepaintSelectedItems();

    void InnerReload(bool retain_state);

    void UpdateContentHeight();

    void OnDataAdded(const ListDataAddedInfo& event_info);
    void HandleDataAdded(const ListDataAddedInfo& event_info);

    void OnDataRemoved(const ListDataRemovedInfo& event_info);
    void HandleDataRemoved(const ListDataRemovedInfo& event_info);

    void OnDataUpdated(const ListDataUpdatedInfo& event_info);
    void HandleDataUpdated(const ListDataUpdatedInfo& event_info);

    void OnDataMoved(const ListDataMovedInfo& event_info);
    void HandleDataMoved(const ListDataMovedInfo& event_info);

    void RefreshItemsIfNeeded();
    float AdjustContentHeight();
    void SetScrollContentHeight(float height);

    void OnSelectionStoreChanged(const ListSelectionStoreChangedInfo& event_info);
    void OnFocusStoreChanged(const ListFocusStoreChangedInfo& event_info);

private:
    std::shared_ptr<ListItemContainer> item_container_;
    std::weak_ptr<ListDataSource> data_source_;
    std::weak_ptr<ListControlDelegate> delegate_;

    SubscriptionSet data_source_subs_;
    SubscriptionSet item_container_subs_;

    Subscription vertical_scroll_bar_sub_;
    Subscription exit_selecting_by_mouse_sub_;

    float current_total_height_{};
    bool disable_on_layout_{};
    bool is_handling_data_source_event_{};
    bool refresh_after_data_source_event_{};

    bool auto_adjust_scroll_bar_small_change_{ true };

    DataSourceChangeEvent data_source_change_event_;
    DelegateChangeEvent delegate_change_event_;
    ItemContainerChangeEvent item_container_change_event_;
    SelectionChangedEvent selection_changed_event_;
    ItemDoubleClickEvent item_double_click_event_;
    ContextMenuEvent context_menu_event_;
};

}