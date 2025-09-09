#pragma once

#include <zaf/internal/property_grid/property_data.h>
#include <zaf/internal/property_grid/split_distance_manager.h>
#include <zaf/control/label.h>
#include <zaf/control/property_grid/value_editor.h>
#include <zaf/control/split_control.h>
#include <zaf/control/tree_item.h>
#include <zaf/rx/subject/subject.h>

namespace zaf::internal {

class PropertyGridItem : public TreeItem {
public:
    PropertyGridItem(
        std::shared_ptr<PropertyData> property_data,
        std::shared_ptr<property_grid::ValueEditor> value_editor,
        std::shared_ptr<SplitDistanceManager> split_distance_manager);

protected:
    void Initialize() override;
    void Layout(const zaf::Rect& previous_rect) override;
    void OnParentChanged(const ParentChangedInfo& event_info) override;

private:
    void InitializeSubControls();
    void InitializeNameLabel();
    void InitializeValueView();
    std::shared_ptr<Label> CreateLabel();
    void InitializeSplitControl();
    void SetAbsoluteSplitDistance(float new_distance);
    void SetFirstPaneMinLength(float max_x);

    void OnEditorValueChanged(const std::shared_ptr<dynamic::Object>& new_value);
    void OnPropertyValueChanged(const std::shared_ptr<PropertyData>& property_data);

private:
    std::shared_ptr<PropertyData> property_data_;

    std::shared_ptr<SplitControl> split_control_;
    std::shared_ptr<Label> name_label_;
    std::shared_ptr<property_grid::ValueEditor> value_editor_;

    std::weak_ptr<SplitDistanceManager> split_distance_manager_;
    bool is_handling_split_distance_event_{};
    bool is_setting_property_value_{};
    bool is_setting_editor_value_{};
};

}