#pragma once

#include <zaf/control/scrollable_control.h>
#include <zaf/control/selection_mode.h>
#include <zaf/internal/no_circular_pointer.h>

namespace zaf {
namespace internal {
class TreeControlImplementation;
}

class ListItemContainer;
class TreeControlDelegate;
class TreeDataSource;

class TreeControl : public ScrollableControl {
public:
    TreeControl();
    ~TreeControl();

    void SetDataSource(const std::shared_ptr<TreeDataSource>& data_source);
    void SetDelegate(const std::shared_ptr<TreeControlDelegate>& delegate);

    void SetSelectionMode(SelectionMode selection_mode);

    std::vector<std::shared_ptr<Object>> GetSelectedItemData() const;
    std::shared_ptr<Object> GetFirstSelectedItemData() const;

protected:
    void Initialize() override;

private:
    std::shared_ptr<ListItemContainer> item_container_;
    internal::NoCircularPointer<TreeDataSource> data_source_;
    internal::NoCircularPointer<TreeControlDelegate> delegate_;
    std::shared_ptr<internal::TreeControlImplementation> implementation_;
};

}