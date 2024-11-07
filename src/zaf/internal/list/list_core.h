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
#include <zaf/internal/list/list_selection_store.h>
#include <zaf/rx/subject.h>
#include <zaf/rx/subscription_host.h>
#include <zaf/window/popup_menu.h>

namespace zaf::internal {

struct ListCoreDataSourceChangedInfo {
    std::shared_ptr<ListDataSource> old_data_source;
};

struct ListCoreDelegateChangedInfo {
    std::shared_ptr<ListControlDelegate> old_delegate;
};

struct ListCoreItemContainerChangedInfo {
    std::shared_ptr<ListItemContainer> old_item_container;
};

struct ListCoreItemDoubleClickInfo {
    std::size_t item_index{};
};


class ListCore : public ListControlPartsBased, SubscriptionHost {
public:
    class InitializeParameters {
    public:
        std::weak_ptr<ListDataSource> data_source;
        std::weak_ptr<ListControlDelegate> delegate;
        std::weak_ptr<ListItemContainer> item_container;
    };

public:
    using ListControlPartsBased::ListControlPartsBased;

    void Initialize(const InitializeParameters& parameters);

    std::shared_ptr<ListDataSource> DataSource() const noexcept;
    void SetDataSource(std::weak_ptr<ListDataSource> data_source);
    Observable<ListCoreDataSourceChangedInfo> DataSourceChangedEvent() const;

    std::shared_ptr<ListControlDelegate> Delegate() const noexcept;
    void SetDelegate(std::weak_ptr<ListControlDelegate> delegate);
    Observable<ListCoreDelegateChangedInfo> DelegateChangedEvent() const;

    std::shared_ptr<ListItemContainer> ItemContainer() const noexcept;
    void SetItemContainer(std::weak_ptr<ListItemContainer> item_container);
    Observable<ListCoreItemContainerChangedInfo> ItemContainerChangedEvent() const;

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

    Observable<None> SelectionChangedEvent() const;
    Observable<ListCoreItemDoubleClickInfo> ItemDoubleClickEvent() const;

private:
    void InstallDataSource(std::weak_ptr<ListDataSource> data_source);
    void InstallDelegate(std::weak_ptr<ListControlDelegate> delegate);
    void InstallItemContainer(std::weak_ptr<ListItemContainer> item_container);
    void OnItemContainerDoubleClick(const DoubleClickInfo& event_info);

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

private:
    std::weak_ptr<ListItemContainer> item_container_;
    Subject<ListCoreItemContainerChangedInfo> item_container_changed_event_;

    std::weak_ptr<ListDataSource> data_source_;
    Subject<ListCoreDataSourceChangedInfo> data_source_changed_event_;

    std::weak_ptr<ListControlDelegate> delegate_;
    Subject<ListCoreDelegateChangedInfo> delegate_changed_event_;

    SubscriptionSet data_source_subs_;
    SubscriptionSet item_container_subs_;

    Subscription vertical_scroll_bar_sub_;
    Subscription exit_selecting_by_mouse_sub_;

    float current_total_height_{};
    bool disable_on_layout_{};
    bool is_handling_data_source_event_{};
    bool refresh_after_data_source_event_{};

    bool auto_adjust_scroll_bar_small_change_{ true };

    Subject<None> selection_changed_event_;
    Subject<ListCoreItemDoubleClickInfo> item_double_click_event_;
};

}