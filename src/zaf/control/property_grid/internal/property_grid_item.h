#pragma once

#include <zaf/control/property_grid/internal/property_grid_data.h>
#include <zaf/control/property_grid/internal/property_grid_split_distance_manager.h>
#include <zaf/control/label.h>
#include <zaf/control/property_grid/value_view.h>
#include <zaf/control/split_control.h>
#include <zaf/control/tree_item.h>
#include <zaf/rx/subject.h>

namespace zaf::internal {

class PropertyGridItem : public TreeItem {
public:
    explicit PropertyGridItem(
        const std::shared_ptr<PropertyGridData>& data,
        const std::shared_ptr<property_grid::ValueView>& value_view,
        const std::shared_ptr<PropertyGridSplitDistanceManager>& split_distance_manager);

protected:
    void Initialize() override;
    void Layout(const zaf::Rect& previous_rect) override;

private:
    void InitializeSubControls();
    void InitializeNameLabel();
    void InitializeValueView();
    std::shared_ptr<Label> CreateLabel() const;
    void InitializeSplitControl();
    void SetSplitDistance(float new_distance);

private:
    std::shared_ptr<PropertyGridData> data_;

    std::shared_ptr<SplitControl> split_control_;
    std::shared_ptr<Label> name_label_;
    std::shared_ptr<property_grid::ValueView> value_view_;

    std::weak_ptr<PropertyGridSplitDistanceManager> split_distance_manager_;
};

}