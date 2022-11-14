#pragma once

#include <zaf/control/scrollable_control.h>

namespace zaf {
namespace internal {
class PropertyGridTreeDataSource;
class PropertyGridTreeDelegate;
class TreeControlImplementation;
}

class PropertyGrid : public ScrollableControl {
public:
    PropertyGrid();

    void SetTargetObject(const std::shared_ptr<Object>& object);

protected:
    void Initialize() override;
    void Layout(const zaf::Rect&) override;

private:
    std::shared_ptr<internal::PropertyGridTreeDataSource> tree_data_source_;
    std::shared_ptr<internal::PropertyGridTreeDelegate> tree_delegate_;
    std::shared_ptr<internal::TreeControlImplementation> tree_implementation_;
};

}