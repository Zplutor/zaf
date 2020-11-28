#pragma once

#include <unordered_set>
#include <zaf/control/internal/list_control/list_control_implementation.h>
#include <zaf/control/internal/tree_control/tree_data_manager.h>
#include <zaf/control/internal/tree_control/tree_index_mapping.h>
#include <zaf/control/list_control_delegate.h>
#include <zaf/control/list_data_source.h>
#include <zaf/control/tree_control_delegate.h>
#include <zaf/control/tree_data_source.h>

namespace zaf::internal {

class TreeControlImplementation : 
    public ListDataSource, 
    public ListControlDelegate,
    public std::enable_shared_from_this<TreeControlImplementation> {

public:
    using DataSourceChangeEvent = std::function<void(const std::shared_ptr<TreeDataSource>&)>;
    using DelegateChangeEvent = std::function<void(const std::shared_ptr<TreeControlDelegate>&)>;
    using SelectionChangeEvent = std::function<void()>;
    using ItemExpandEvent = std::function<void(const std::shared_ptr<Object>&)>;
    using ItemCollapseEvent = std::function<void(const std::shared_ptr<Object>&)>;

    class InitializeParameters {
    public:
        std::shared_ptr<ListItemContainer> item_container;
        std::shared_ptr<TreeDataSource> data_source;
        std::shared_ptr<TreeControlDelegate> delegate;
        DataSourceChangeEvent data_source_change_event;
        DelegateChangeEvent delegate_change_event;
        ListControlImplementation::ItemContainerChangeEvent item_container_change_event;
        SelectionChangeEvent selection_change_event;
        ItemExpandEvent item_expand_event;
        ItemCollapseEvent item_collapse_event;
    };

public:
    TreeControlImplementation(ScrollableControl& owner);
    ~TreeControlImplementation() = default;

    TreeControlImplementation(const TreeControlImplementation&) = delete;
    TreeControlImplementation& operator=(const TreeControlImplementation&) = delete;

    internal::ListControlImplementation& GetListImplementation() const {
        return *list_implementation_;
    }

    void Initialize(const InitializeParameters& parameters);

    void SetDataSource(const std::shared_ptr<TreeDataSource>& data_source);
    void SetDelegate(const std::shared_ptr<TreeControlDelegate>& delegate);

    void Reload();

    std::vector<std::shared_ptr<Object>> GetAllSelectedItems() const;
    std::shared_ptr<Object> GetFirstSelectedItem() const;

    void SelectItem(const std::shared_ptr<Object>& data);
    void UnselectItem(const std::shared_ptr<Object>& data);

    std::size_t GetDataCount() override;
    std::shared_ptr<Object> GetDataAtIndex(std::size_t index) override;
    
    bool HasVariableItemHeight() override;

    float EstimateItemHeight(
        std::size_t item_index,
        const std::shared_ptr<Object>& item_data) override;

    std::wstring GetItemText(
        std::size_t item_index,
        const std::shared_ptr<Object>& item_data) override;

    std::shared_ptr<ListItem> CreateItem(
        std::size_t item_index,
        const std::shared_ptr<Object>& item_data) override;

    void LoadItem(
        const std::shared_ptr<ListItem>& item,
        std::size_t item_index,
        const std::shared_ptr<Object>& item_data) override;

    void OnItemExpandChange(const std::shared_ptr<TreeItem>& item, bool is_expanded);

private:
    using TreeDataSet = std::unordered_set<std::shared_ptr<Object>, TreeDataHash, TreeDataEqual>;

private:
    void InitializeListImplementation(const InitializeParameters& parameters);
    void InstallDataSource(const std::shared_ptr<TreeDataSource>& data_source);
    void RegisterDataSourceEvents();
    void UnregisterDataSourceEvents();
    void InstallDelegate(const std::shared_ptr<TreeControlDelegate>& delegate);

    void ReloadRootNode();

    std::optional<float> InnerEstimateItemHeight(
        std::size_t item_index,
        const std::shared_ptr<Object>& item_data);

    std::shared_ptr<ListItem> InnerCreateListItem(
        std::size_t item_index,
        const std::shared_ptr<Object>& item_data);

    void SetItemExpandState(
        const std::shared_ptr<TreeItem>& item, 
        const std::shared_ptr<Object>& item_data,
        const IndexPath& index_path);

    bool IsIndexPathExpanded(const IndexPath& index_path) const;

    void SetItemSelectionState(
        const std::shared_ptr<TreeItem>& item, 
        const std::shared_ptr<Object>& item_data);

    void ExpandItem(std::size_t list_item_index);
    bool ExpandItemAtIndexPath(
        const IndexPath& index_path, 
        std::shared_ptr<Object>& expanded_data, 
        std::size_t& expanded_count);
    std::size_t ExpandItemWithTreeNode(const TreeNode& node, const IndexPath& node_index_path);
    bool ExpandNewItem(
        const IndexPath& index_path,
        std::shared_ptr<Object>& expanded_data,
        std::size_t& expanded_count);
    void CollapseItem(std::size_t list_item_index);

    bool GetParentDataAndChildIndex(
        std::size_t list_index,
        std::shared_ptr<Object>& parent_data,
        std::size_t& child_index);

    bool GetParentDataAndChildIndex(
        const IndexPath& index_path,
        std::shared_ptr<Object>& parent_data,
        std::size_t& child_index);

    void OnListSelectionChange(
        ListSelectionChangeReason reason,
        std::size_t index,
        std::size_t count);

    void NotifySelectionChange();

    void ModifySelection(std::size_t index, std::size_t count, bool is_replace);
    void RemoveSelection(std::size_t index, std::size_t count);

    void OnDataAdd(const TreeDataSourceDataAddInfo& event_info);
    std::size_t GetChildListIndex(
        const IndexPath& parent_index_path, 
        std::size_t child_index) const;
    void OnDataRemove(const TreeDataSourceDataRemoveInfo& event_info);
    void OnDataUpdate(const TreeDataSourceDataUpdateInfo& event_info);
    std::vector<std::size_t> GetChildrenListIndexes(
        const std::shared_ptr<Object>& parent_data, 
        std::size_t index, 
        std::size_t count);

private:
    std::shared_ptr<internal::ListControlImplementation> list_implementation_;
    std::weak_ptr<TreeDataSource> data_source_;
    std::weak_ptr<TreeControlDelegate> delegate_;

    DataSourceChangeEvent data_source_change_event_;
    DelegateChangeEvent delegate_change_event_;
    SelectionChangeEvent selection_change_event_;
    ItemExpandEvent item_expand_event_;
    ItemCollapseEvent item_collapse_event_;

    SubscriptionHolder data_source_subscriptions_;
    
    TreeIndexMapping tree_index_mapping_;
    TreeDataManager tree_data_manager_;
    TreeDataSet expanded_data_set_;
    TreeDataSet selected_data_set_;
};

}