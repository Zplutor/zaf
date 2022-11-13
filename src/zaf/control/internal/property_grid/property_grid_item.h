#pragma once

#include <zaf/control/internal/property_grid/property_grid_data.h>
#include <zaf/control/internal/property_grid/property_grid_split_distance_manager.h>
#include <zaf/control/label.h>
#include <zaf/control/split_control.h>
#include <zaf/control/tree_item.h>
#include <zaf/rx/subject.h>

namespace zaf::internal {

class PropertyGridItem : public TreeItem {
public:
    explicit PropertyGridItem(
        const std::shared_ptr<PropertyGridData>& data,
        const std::shared_ptr<PropertyGridSplitDistanceManager>& split_distance_manager);

protected:
    void Initialize() override;
    void Layout(const zaf::Rect& previous_rect) override;

private:
    void InitializeSubControls();
    void InitializeNameLabel();
    void InitializeValueLabel();
    std::shared_ptr<Label> CreateLabel() const;
    void InitializeSplitControl();

private:
    std::shared_ptr<PropertyGridData> data_;

    std::shared_ptr<SplitControl> split_control_;
    std::shared_ptr<Label> name_label_;
    std::shared_ptr<Label> value_label_;

    std::weak_ptr<PropertyGridSplitDistanceManager> split_distance_manager_;
};

}