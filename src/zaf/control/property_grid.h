#pragma once

#include <zaf/control/property_grid/type_config_factory.h>
#include <zaf/control/scrollable_control.h>

namespace zaf {
namespace internal {
class PropertyGridSplitDistanceManager;
class PropertyGridTreeDataSource;
class PropertyGridTreeDelegate;
class TreeControlImplementation;
}

class PropertyGrid : public ScrollableControl {
public:
    ZAF_DECLARE_TYPE;

public:
    PropertyGrid();

    std::shared_ptr<Object> TargetObject() const {
        return target_object_;
    }

    void SetTargetObject(const std::shared_ptr<Object>& object);

    void SetTypeConfigFactory(const std::shared_ptr<property_grid::TypeConfigFactory>& factory);

    void Reload();

protected:
    void Initialize() override;
    void Layout(const zaf::Rect&) override;

private:
    void ReCreateDataSource();
    void ReCreateDelegate();

private:
    std::shared_ptr<internal::PropertyGridSplitDistanceManager> split_distance_manager_;
    std::shared_ptr<Object> target_object_;
    std::shared_ptr<property_grid::TypeConfigFactory> type_config_factory_;
    std::shared_ptr<internal::PropertyGridTreeDataSource> tree_data_source_;
    std::shared_ptr<internal::PropertyGridTreeDelegate> tree_delegate_;
    std::shared_ptr<internal::TreeControlImplementation> tree_implementation_;

};

}