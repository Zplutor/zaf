#pragma once

#include <zaf/control/scrollable_control.h>
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

protected:
    void Initialize() override;

private:
    std::shared_ptr<ListItemContainer> item_container_;
    internal::NoCircularPointer<TreeDataSource> data_source_;
    internal::NoCircularPointer<TreeControlDelegate> delegate_;
    std::shared_ptr<internal::TreeControlImplementation> implementation_;
};

}