#pragma once

#include <zaf/control/internal/list_control/list_control_implementation.h>
#include <zaf/control/internal/tree_control/tree_data_manager.h>
#include <zaf/control/internal/tree_control/tree_index_mapping.h>
#include <zaf/control/internal/tree_control/tree_visitor.h>
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
        std::weak_ptr<TreeDataSource> data_source;
        std::weak_ptr<TreeControlDelegate> delegate;
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

    void SetDataSource(const std::weak_ptr<TreeDataSource>& data_source);
    void SetDelegate(const std::weak_ptr<TreeControlDelegate>& delegate);

    void Reload();

    std::vector<std::shared_ptr<Object>> GetAllSelectedItems() const;
    std::shared_ptr<Object> GetFirstSelectedItem() const;

    void VisitExpandedTree(TreeVisitor& visitor) const;

    void SelectItem(const std::shared_ptr<Object>& data);
    void UnselectItem(const std::shared_ptr<Object>& data);

    void ExpandItem(const std::shared_ptr<Object>& data);
    void CollapseItem(const std::shared_ptr<Object>& data);

    void ScrollToItem(const std::shared_ptr<Object>& data);

    void ReloadItem(const std::shared_ptr<Object>& data);

    std::size_t GetDataCount() override;
    std::shared_ptr<Object> GetDataAtIndex(std::size_t index) override;
    
    bool HasVariableItemHeight() override;

    float EstimateItemHeight(
        std::size_t item_index,
        const std::shared_ptr<Object>& item_data) override;

    float GetItemSpacing() override;

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

    bool ChangeItemExpandState(const std::shared_ptr<TreeItem>& item, bool new_is_expanded);

private:
    void InitializeListImplementation(const InitializeParameters& parameters);
    void InstallDataSource(const std::weak_ptr<TreeDataSource>& data_source);
    void RegisterDataSourceEvents();
    void UnregisterDataSourceEvents();
    void InstallDelegate(const std::weak_ptr<TreeControlDelegate>& delegate);

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

    void VisitExpandedTreeNode(const TreeNode& node, TreeVisitor& visitor) const;

    void SetItemSelectionState(
        const std::shared_ptr<TreeItem>& item, 
        const std::shared_ptr<Object>& item_data);

    bool ExpandItemUI(
        const IndexPath& index_path,
        const std::optional<std::size_t>& list_index, 
        bool update_item);
    bool ExpandItemData(
        const IndexPath& index_path, 
        std::shared_ptr<Object>& expanded_data, 
        std::size_t& expanded_count);
    std::size_t ExpandItemRecursively(
        TreeDataSource& data_source,
        TreeNodeExpander& node_expander,
        const IndexPath& node_index_path);
    bool CollapseItemUI(
        const IndexPath& index_path,
        const std::optional<std::size_t>& list_index,
        bool update_item);
    void OnListItemDoubleClick(std::size_t list_index);

    void UpdateItem(const IndexPath& index_path);
    void CheckIfItemHasChildren(const IndexPath& index_path, std::size_t list_index);

    void OnListSelectionChange(
        ListSelectionChangeReason reason,
        std::size_t index,
        std::size_t count);

    void NotifySelectionChange();

    void ModifySelection(std::size_t index, std::size_t count, bool is_replace);
    void RemoveSelection(std::size_t index, std::size_t count);

    void OnDataAdd(const TreeDataSourceDataAddInfo& event_info);
    std::optional<std::size_t> GetChildListIndex(
        const IndexPath& parent_index_path, 
        std::size_t child_index) const;
    void OnDataRemove(const TreeDataSourceDataRemoveInfo& event_info);
    void OnDataUpdate(const TreeDataSourceDataUpdateInfo& event_info);
    std::vector<std::size_t> UpdateChildItem(
        const std::shared_ptr<Object>& parent_data, 
        std::size_t index, 
        std::size_t count);

    std::optional<std::size_t> GetDataListIndex(const std::shared_ptr<Object>& data) const;

    bool GetParentDataAndChildIndex(
        std::size_t list_index,
        std::shared_ptr<Object>& parent_data,
        std::size_t& child_index);

    bool GetParentDataAndChildIndex(
        const IndexPath& index_path,
        std::shared_ptr<Object>& parent_data,
        std::size_t& child_index);

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
};

}