#pragma once

#include <zaf/base/non_copyable.h>
#include <zaf/internal/tree/tree_data_manager.h>
#include <zaf/internal/tree/tree_index_mapping.h>
#include <zaf/internal/tree/tree_visitor.h>
#include <zaf/control/list_control_delegate.h>
#include <zaf/control/list_data_source.h>
#include <zaf/control/tree_control_delegate.h>
#include <zaf/control/tree_data_source.h>
#include <zaf/control/tree_item_container.h>
#include <zaf/internal/list/list_control_parts_context.h>

namespace zaf::internal {

class TreeCore : 
    public ListDataSource, 
    public ListControlDelegate,
    public std::enable_shared_from_this<TreeCore>,
    rx::DisposableHost {

public:
    using DataSourceChangeEvent = std::function<void(const std::shared_ptr<TreeDataSource>&)>;
    using DelegateChangeEvent = std::function<void(const std::shared_ptr<TreeControlDelegate>&)>;
    using SelectionChangeEvent = std::function<void()>;
    using ItemExpandEvent = std::function<void(const std::shared_ptr<Object>&)>;
    using ItemCollapseEvent = std::function<void(const std::shared_ptr<Object>&)>;

    class InitializeParameters {
    public:
        std::weak_ptr<TreeItemContainer> item_container;
        std::weak_ptr<TreeDataSource> data_source;
        std::weak_ptr<TreeControlDelegate> delegate;
        DataSourceChangeEvent data_source_change_event;
        DelegateChangeEvent delegate_change_event;
        SelectionChangeEvent selection_change_event;
        ItemExpandEvent item_expand_event;
        ItemCollapseEvent item_collapse_event;
    };

public:
    TreeCore(ScrollBox& owner);
    ~TreeCore() = default;

    ListControlPartsContext& ListParts() {
        return list_parts_;
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

    std::size_t GetDataCount() const override;
    std::shared_ptr<Object> GetDataAtIndex(std::size_t index) const override;
    
    bool HasVariableItemHeight() override;

    float EstimateItemHeight(
        std::size_t item_index,
        const std::shared_ptr<Object>& item_data) override;

    float GetItemSpacing() override;

    std::shared_ptr<ListItem> CreateItem(
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
    void OnListItemDoubleClick(const ListCoreItemDoubleClickInfo& event_info);
    void OnChangeExpandStateByArrowKeys(bool is_to_expand);

    void UpdateItem(const IndexPath& index_path);
    void CheckIfItemHasChildren(const IndexPath& index_path, std::size_t list_index);

    void OnListSelectionStoreChanged(const ListSelectionStoreChangedInfo& event_info);
    void OnListSelectionChange(None);

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
    ListControlPartsContext list_parts_;
    std::weak_ptr<TreeDataSource> data_source_;
    std::weak_ptr<TreeControlDelegate> delegate_;

    DataSourceChangeEvent data_source_change_event_;
    DelegateChangeEvent delegate_change_event_;
    SelectionChangeEvent selection_change_event_;
    ItemExpandEvent item_expand_event_;
    ItemCollapseEvent item_collapse_event_;

    rx::DisposeBag item_container_subs_;
    rx::DisposeBag data_source_subscriptions_;
    
    TreeIndexMapping tree_index_mapping_;
    TreeDataManager tree_data_manager_;
};

}