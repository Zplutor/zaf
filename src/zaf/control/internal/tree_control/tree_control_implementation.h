#pragma once

#include <zaf/control/internal/list_control/list_control_implementation.h>
#include <zaf/control/internal/tree_control/tree_data.h>
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
    class InitializeParameters {
    public:
        std::shared_ptr<ListItemContainer> item_container;
        std::shared_ptr<TreeDataSource> data_source;
        std::shared_ptr<TreeControlDelegate> delegate;
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
    void InitializeListImplementation(const InitializeParameters& parameters);
    void InstallDataSource(const std::shared_ptr<TreeDataSource>& data_source);
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

    void ExpandItem(std::size_t list_item_index);
    void CollapseItem(std::size_t list_item_index);

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

    std::map<IndexPath, std::shared_ptr<Object>> expanded_node_data_;
    TreeData tree_data_;
};

}