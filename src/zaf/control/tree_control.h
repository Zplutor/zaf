#pragma once

#include <zaf/control/scrollable_control.h>
#include <zaf/control/selection_mode.h>
#include <zaf/internal/no_circular_pointer.h>
#include <zaf/rx/observable.h>

namespace zaf {
namespace internal {
class TreeControlImplementation;
}

class ListItemContainer;
class TreeControlDelegate;
class TreeControlSelectionChangeInfo;
class TreeDataSource;

class TreeControl : public ScrollableControl {
public:
    ZAF_DECLARE_REFLECTION_TYPE

public:
    TreeControl();
    ~TreeControl();

    void SetDataSource(const std::shared_ptr<TreeDataSource>& data_source);
    void SetDelegate(const std::shared_ptr<TreeControlDelegate>& delegate);

    void SetSelectionMode(SelectionMode selection_mode);

    std::vector<std::shared_ptr<Object>> GetAllSelectedItemData() const;
    std::shared_ptr<Object> GetFirstSelectedItemData() const;

    void SelectItemWithData(const std::shared_ptr<Object>& data);
    void UnselectItemWithData(const std::shared_ptr<Object>& data);

    Observable<TreeControlSelectionChangeInfo> SelectionChangeEvent();

protected:
    void Initialize() override;

    virtual void DataSourceChange(const std::shared_ptr<TreeDataSource>& previous_data_source) { }
    virtual void DelegateChange(const std::shared_ptr<TreeControlDelegate>& previous_delegate) { }
    virtual void ItemContainerChange(
        const std::shared_ptr<ListItemContainer>& previous_item_container) { }

private:
    void SelectionChange();

private:
    std::shared_ptr<ListItemContainer> item_container_;
    internal::NoCircularPointer<TreeDataSource> data_source_;
    internal::NoCircularPointer<TreeControlDelegate> delegate_;
    std::shared_ptr<internal::TreeControlImplementation> implementation_;
};


class TreeControlSelectionChangeInfo {
public:
    std::shared_ptr<TreeControl> tree_control;
};

}