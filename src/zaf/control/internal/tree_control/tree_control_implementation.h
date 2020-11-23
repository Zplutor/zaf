#pragma once

#include <unordered_set>
#include <zaf/control/internal/list_control/list_control_implementation.h>
#include <zaf/control/internal/tree_control/tree_data.h>
#include <zaf/control/internal/tree_control/tree_data_manager.h>
#include <zaf/control/internal/tree_control/tree_selection_manager.h>
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
    using SelectionChangeEvent = std::function<void()>;

    class InitializeParameters {
    public:
        std::shared_ptr<ListItemContainer> item_container;
        std::shared_ptr<TreeDataSource> data_source;
        std::shared_ptr<TreeControlDelegate> delegate;
        SelectionChangeEvent selection_change_event;
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

    std::vector<std::shared_ptr<Object>> GetAllSelectedItemData() const;
    std::shared_ptr<Object> GetFirstSelectedItemData() const;

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
        const std::shared_ptr<Object>& parent_data,
        std::size_t item_index_in_parent);

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

    SelectionChangeEvent selection_change_event_;
    SubscriptionHolder data_source_subscriptions_;
    
    TreeData tree_data_;
    TreeDataManager tree_data_manager_;
    TreeDataSet expanded_data_set_;
    TreeSelectionManager selection_manager_;
};

}