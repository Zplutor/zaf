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
#include <zaf/internal/list/list_selection_store.h>
#include <zaf/rx/subscription_host.h>
#include <zaf/window/popup_menu.h>

namespace zaf::internal {

class ListControlPartContext;

class ListControlCore :
    public std::enable_shared_from_this<ListControlCore>,
    SubscriptionHost,
    NonCopyableNonMovable {

public:
    using DataSourceChangeEvent = std::function<void(const std::shared_ptr<ListDataSource>&)>;
    using DelegateChangeEvent = std::function<void(const std::shared_ptr<ListControlDelegate>&)>;
    using ItemContainerChangeEvent = std::function<void(const std::shared_ptr<ListItemContainer>&)>;
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
    explicit ListControlCore(zaf::ScrollBox& owner);
    ~ListControlCore();

    void Initialize(const InitializeParameters& parameters);

    zaf::ScrollBox& ScrollBox() {
        return owner_;
    }

    std::shared_ptr<ListDataSource> DataSource() const noexcept {
        return data_source_.lock();
    }

    void SetDataSource(const std::weak_ptr<ListDataSource>& data_source);

    std::shared_ptr<ListControlDelegate> Delegate() const noexcept {
        return delegate_.lock();
    }
    void SetDelegate(const std::weak_ptr<ListControlDelegate>& delegate);

    const std::shared_ptr<ListItemContainer>& ItemContainer() const {
        return item_container_;
    }
    void SetItemContainer(const std::shared_ptr<ListItemContainer>& item_container);

    SelectionMode GetSelectionMode() const noexcept;
    void SetSelectionMode(SelectionMode mode);

    void OnLayout();
    void OnVerticalScrollBarChange(const std::shared_ptr<ScrollBar>& previous_scroll_bar);

    void Reload();
    void UnselectAllItems();
    void SelectItemAtIndex(std::size_t index);
    void UnselectItemAtIndex(std::size_t index);

    std::size_t GetItemCount();
    std::size_t GetSelectedItemCount();
    std::optional<std::size_t> GetFirstSelectedItemIndex() const noexcept;
    std::vector<std::size_t> GetAllSelectedItemIndexes();
    bool IsItemSelectedAtIndex(std::size_t index);

    void ScrollToItemAtIndex(std::size_t index);

    std::optional<std::size_t> FindItemIndexAtPosition(const Point& position);
    std::optional<std::size_t> GetListItemIndex(const std::shared_ptr<ListItem>& item);

    bool AutoAdjustScrollBarSmallChange() const {
        return auto_adjust_scroll_bar_small_change_;
    }

    void SetAutoAdjustScrollBarSmallChange(bool value) {
        auto_adjust_scroll_bar_small_change_ = value;
        if (auto_adjust_scroll_bar_small_change_) {
            AdjustScrollBarSmallChange();
        }
    }

    const ListControlPartContext& PartContext() const {
        return *part_context_;
    }

private:
    void InstallDataSource(const std::weak_ptr<ListDataSource>& data_source);
    void InstallDelegate(const std::weak_ptr<ListControlDelegate>& delegate);
    void InstallItemContainer(const std::shared_ptr<ListItemContainer>& item_container);
    void OnItemContainerDoubleClick(const DoubleClickInfo& event_info);
    void OnItemContainerMouseUp(const MouseUpInfo& event_info);
    void OnItemContainerGainedFocus(const FocusGainedInfo& event_info);
    void OnItemContainerLostFocus(const FocusLostInfo& event_info);

    void RegisterScrollBarEvents();
    void UnregisterScrollBarEvents(const std::shared_ptr<ScrollBar>& scroll_bar);
    void AdjustScrollBarSmallChange();

    void RegisterDataSourceEvents();

    void InnerReload(bool retain_state);

    void UpdateContentHeight();
    void RecoverLastFocusedItem(const std::vector<std::shared_ptr<ListItem>>& items);

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

private:
    std::unique_ptr<ListControlPartContext> part_context_;
    zaf::ScrollBox& owner_;

    std::shared_ptr<ListItemContainer> item_container_;
    std::weak_ptr<ListDataSource> data_source_;
    std::weak_ptr<ListControlDelegate> delegate_;

    SubscriptionSet data_source_subs_;
    SubscriptionSet item_container_subs_;

    std::weak_ptr<Object> last_focused_item_data_;

    Subscription vertical_scroll_bar_sub_;
    Subscription exit_handle_mouse_event_sub_;

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