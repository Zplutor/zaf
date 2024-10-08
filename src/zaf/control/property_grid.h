#pragma once

#include <zaf/control/property_grid_node.h>
#include <zaf/control/scroll_box.h>

namespace zaf {
namespace internal {
class TreeControlImplementation;
}

namespace property_grid {
namespace internal {
class DataSource;
class Delegate;
class SplitDistanceManager;
}
class TypeConfigFactory;
}

class PropertyGrid : public ScrollBox {
public:
    ZAF_OBJECT;

public:
    PropertyGrid();

    std::shared_ptr<Object> TargetObject() const {
        return target_object_;
    }

    void SetTargetObject(const std::shared_ptr<Object>& object);

    void SetTypeConfigFactory(const std::shared_ptr<property_grid::TypeConfigFactory>& factory);

    void RefreshValues();
    void Reload();

    PropertyGridNode GetExpandedNodeTree() const;
    void ExpandNodeTree(const PropertyGridNode& tree);

protected:
    void Initialize() override;
    void Layout(const zaf::Rect&) override;

private:
    void ReCreateDataSource();
    void ReCreateDelegate();
    void ExpandChildNodes(const PropertyGridNode& node, const std::shared_ptr<Object>& node_data);

private:
    std::shared_ptr<property_grid::internal::SplitDistanceManager> split_distance_manager_;
    std::shared_ptr<Object> target_object_;
    std::shared_ptr<property_grid::TypeConfigFactory> type_config_factory_;
    std::shared_ptr<property_grid::internal::DataSource> data_source_;
    std::shared_ptr<property_grid::internal::Delegate> delegate_;
    std::shared_ptr<internal::TreeControlImplementation> tree_implementation_;
};

ZAF_OBJECT_BEGIN(PropertyGrid);
ZAF_OBJECT_END;

}