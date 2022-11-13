#pragma once

#include <zaf/control/scrollable_control.h>

namespace zaf {
namespace internal {
class PropertyGridDataSource;
class PropertyGridDelegate;
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
    std::shared_ptr<internal::PropertyGridDataSource> data_source_;
    std::shared_ptr<internal::PropertyGridDelegate> delegate_;
    std::shared_ptr<internal::TreeControlImplementation> implementation_;
};

}