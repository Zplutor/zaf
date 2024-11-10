#pragma once

#include <zaf/control/property_grid_node.h>
#include <zaf/control/scroll_box.h>
#include <zaf/internal/no_self_ref_ptr.h>

namespace zaf::internal {
class TreeControlImplementation;
class PropertyGridDataManager;
class PropertyGridItemManager;
class SplitDistanceManager;
}

namespace zaf {

class PropertyGridDelegate;

class PropertyGrid : public ScrollBox {
public:
    ZAF_OBJECT;

public:
    PropertyGrid();

    const std::shared_ptr<Object>& TargetObject() const {
        return target_object_;
    }

    void SetTargetObject(std::shared_ptr<Object> object);

    void SetDelegate(std::shared_ptr<PropertyGridDelegate> delegate);

    void RefreshValues();
    void Reload();

    PropertyGridNode GetExpandedNodeTree() const;
    void ExpandNodeTree(const PropertyGridNode& tree);

protected:
    void Initialize() override;
    void Layout(const zaf::Rect&) override;

    void OnMouseDown(const MouseDownInfo& event_info) override;
    void OnMouseMove(const MouseMoveInfo& event_info) override;
    void OnMouseUp(const MouseUpInfo& event_info) override;
    void OnKeyDown(const KeyDownInfo& event_info) override;

    void OnFocusGained(const FocusGainedInfo& event_info) override;
    void OnFocusLost(const FocusLostInfo& event_info) override;

private:
    void ReCreateDataSource();
    void ReCreateDelegate();
    void ExpandChildNodes(const PropertyGridNode& node, const std::shared_ptr<Object>& node_data);

private:
    std::shared_ptr<internal::SplitDistanceManager> split_distance_manager_;
    std::shared_ptr<Object> target_object_;
    internal::NoSelfRefPtr<PropertyGridDelegate> delegate_;
    std::shared_ptr<internal::PropertyGridDataManager> data_manager_;
    std::shared_ptr<internal::PropertyGridItemManager> item_manager_;
    std::shared_ptr<internal::TreeControlImplementation> tree_implementation_;
};

ZAF_OBJECT_BEGIN(PropertyGrid);
ZAF_OBJECT_END;

}