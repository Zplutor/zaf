#pragma once

#include <deque>
#include <zaf/control/internal/list_control/list_control_item_selection_manager.h>
#include <zaf/control/list_control_delegate.h>
#include <zaf/control/list_data_source.h>
#include <zaf/control/list_item.h>
#include <zaf/control/list_item_container.h>
#include <zaf/control/scroll_bar.h>
#include <zaf/control/scroll_box.h>
#include <zaf/control/selection_mode.h>
#include <zaf/rx/subscription_set.h>

namespace zaf::internal {

class ListControlItemHeightManager;
class ListControlItemSelectionManager;

enum class ListSelectionChangeReason {
    ItemChange,
    ReplaceSelection,
    AddSelection,
    RemoveSelection,
};

class ListControlImplementation : public std::enable_shared_from_this<ListControlImplementation> {
public:
    using DataSourceChangeEvent = std::function<void(const std::shared_ptr<ListDataSource>&)>;
    using DelegateChangeEvent = std::function<void(const std::shared_ptr<ListControlDelegate>&)>;
    using ItemContainerChangeEvent = std::function<void(const std::shared_ptr<ListItemContainer>&)>;
    using SelectionChangeEvent = std::function<void(
        ListSelectionChangeReason change_type, 
        std::size_t index, 
        std::size_t count)>;
    using ItemDoubleClickEvent = std::function<void(std::size_t)>;

    class InitializeParameters {
    public:
        std::weak_ptr<ListDataSource> data_source;
        std::weak_ptr<ListControlDelegate> delegate;
        std::shared_ptr<ListItemContainer> item_container;
        DataSourceChangeEvent data_source_change_event;
        DelegateChangeEvent delegate_change_event;
        ItemContainerChangeEvent item_container_change_event;
        SelectionChangeEvent selection_change_event;
        ItemDoubleClickEvent item_double_click_event;
    };

public:
    ListControlImplementation(ScrollBox& owner);
    ~ListControlImplementation();

    ListControlImplementation(const ListControlImplementation&) = delete;
    ListControlImplementation& operator=(const ListControlImplementation&) = delete;

    void Initialize(const InitializeParameters& parameters);
    void SetDataSource(const std::weak_ptr<ListDataSource>& data_source);
    void SetDelegate(const std::weak_ptr<ListControlDelegate>& delegate);
    void SetItemContainer(const std::shared_ptr<ListItemContainer>& item_container);

    SelectionMode GetSelectionMode() const {
        return selection_mode_;
    }

    void SetSelectionMode(SelectionMode mode);

    void OnLayout();
    void OnVerticalScrollBarChange(const std::shared_ptr<ScrollBar>& previous_scroll_bar);

    void Reload();
    void SelectAllItems();
    void UnselectAllItems();
    void SelectItemAtIndex(std::size_t index);
    void UnselectItemAtIndex(std::size_t index);

    std::size_t GetItemCount();
    std::size_t GetSelectedItemCount();
    std::optional<std::size_t> GetFirstSelectedItemIndex();
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

private:
    friend class ListControlExtendedMultipleSelectStrategy;
    friend class ListControlSingleSelectStrategy;
    friend class ListControlSimpleMultipleSelectStrategy;

    void ReplaceSelection(std::size_t index, std::size_t count);
    bool RevertSelection(std::size_t index);
    void AddSelection(std::size_t index, std::size_t count);
    void RemoveSelection(std::size_t index, std::size_t count);
    void NotifySelectionChange(
        ListSelectionChangeReason change_type, 
        std::size_t index,
        std::size_t count);

private:
    void InstallDataSource(const std::weak_ptr<ListDataSource>& data_source);
    void InstallDelegate(const std::weak_ptr<ListControlDelegate>& delegate);
    void InstallItemContainer(const std::shared_ptr<ListItemContainer>& item_container);
    void OnItemContainerDoubleClick(const DoubleClickInfo& event_info);
    void OnItemContainerGainedFocus(const FocusGainedInfo& event_info);
    void OnItemContainerLostFocus(const FocusLostInfo& event_info);

    void RegisterScrollBarEvents();
    void UnregisterScrollBarEvents(const std::shared_ptr<ScrollBar>& scroll_bar);
    void AdjustScrollBarSmallChange();

    void RegisterDataSourceEvents();
    void UnregisterDataSourceEvents();

    void InnerReload(bool retain_state);

    void UpdateContentHeight();
    void UpdateVisibleItems();
    void GetVisibleItemsRange(std::size_t& index, std::size_t& count);
    void AdjustVisibleItems(
        std::size_t new_index,
        std::size_t new_count,
        bool remove_head,
        std::size_t head_change_count,
        bool remove_tail,
        std::size_t tail_change_count);
    void RemoveHeadVisibleItems(std::size_t count);
    void RemoveTailVisibleItems(std::size_t count);
    std::vector<std::shared_ptr<ListItem>> CreateItems(std::size_t index, std::size_t count);
    std::shared_ptr<ListItem> CreateItem(std::size_t index);
    void RecoverLastFocusedItem(const std::vector<std::shared_ptr<ListItem>>& items);

    void OnItemAdd(const ListDataSourceDataAddInfo& event_info);
    void HandleItemAdd(const ListDataSourceDataAddInfo& event_info);
    void AddItemsBeforeVisibleItems(
        std::size_t index, 
        std::size_t count, 
        float position_difference);
    void AddItemsInMiddleOfVisibleItems(
        std::size_t index, 
        std::size_t count, 
        float position_difference);

    void OnItemRemove(const ListDataSourceDataRemoveInfo& event_info);
    void HandleItemRemove(const ListDataSourceDataRemoveInfo& event_info);
    void RemoveItemsBeforeVisibleItems(
        std::size_t index, 
        std::size_t count, 
        float position_difference);
    void RemoveItemsInMiddleOfVisibleItems(
        std::size_t index, 
        std::size_t count, 
        float position_difference);

    void OnItemUpdate(const ListDataSourceDataUpdateInfo& event_info);
    void HandleItemUpdate(const ListDataSourceDataUpdateInfo& event_info);
    void AdjustVisibleItemPositionsByUpdatingItems(
        std::size_t index, 
        std::size_t count, 
        float position_difference);
    void UpdateVisibleItemsByUpdatingItems(std::size_t index, std::size_t count);

    void RefreshItemsIfNeeded();
    float AdjustContentHeight();
    void SetScrollContentHeight(float height);
    void AdjustVisibleItemPositions(std::size_t begin_adjust_index, float difference);

    void ChangeSelection(std::size_t index, std::size_t count, bool is_add);

    std::shared_ptr<internal::ListControlSelectStrategy> CreateSelectStrategy();

private:
    ScrollBox& owner_;

    std::shared_ptr<ListItemContainer> item_container_;
    std::weak_ptr<ListDataSource> data_source_;
    std::weak_ptr<ListControlDelegate> delegate_;

    SubscriptionSet data_source_subscriptions_;
    SubscriptionSet item_container_subscriptions_;

    std::shared_ptr<ListControlItemHeightManager> item_height_manager_;
    ListControlItemSelectionManager item_selection_manager_;
    std::weak_ptr<Object> last_focused_item_data_;

    Subscription vertical_scroll_bar_subscription_;

    float current_total_height_{};
    std::size_t first_visible_item_index_{};
    std::deque<std::shared_ptr<ListItem>> visible_items_;
    bool disable_on_layout_{};
    bool is_handling_data_source_event_{};
    bool refresh_after_data_source_event_{};

    SelectionMode selection_mode_{ SelectionMode::Single };
    bool auto_adjust_scroll_bar_small_change_{ true };

    DataSourceChangeEvent data_source_change_event_;
    DelegateChangeEvent delegate_change_event_;
    ItemContainerChangeEvent item_container_change_event_;
    SelectionChangeEvent selection_change_event_;
    ItemDoubleClickEvent item_double_click_event_;
};

}