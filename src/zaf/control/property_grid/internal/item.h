#pragma once

#include <zaf/base/auto_reset_value.h>
#include <zaf/control/property_grid/internal/data.h>
#include <zaf/control/property_grid/internal/split_distance_manager.h>
#include <zaf/control/label.h>
#include <zaf/control/property_grid/value_view.h>
#include <zaf/control/split_control.h>
#include <zaf/control/tree_item.h>
#include <zaf/rx/subject.h>

namespace zaf::property_grid::internal {

class Item : public TreeItem {
public:
    Item(
        const std::shared_ptr<Data>& data,
        const std::shared_ptr<ValueView>& value_view,
        const std::shared_ptr<SplitDistanceManager>& split_distance_manager);

protected:
    void Initialize() override;
    void Layout(const zaf::Rect& previous_rect) override;
    void OnParentChanged(const ParentChangedInfo& event_info) override;
    void RecoverFocus() override;

private:
    void InitializeSubControls();
    void InitializeNameLabel();
    void InitializeValueView();
    std::shared_ptr<Label> CreateLabel() const;
    void InitializeSplitControl();
    void SetAbsoluteSplitDistance(float new_distance);
    void SetFirstPaneMinLength(float max_x);

private:
    std::shared_ptr<Data> data_;

    std::shared_ptr<SplitControl> split_control_;
    std::shared_ptr<Label> name_label_;
    std::shared_ptr<property_grid::ValueView> value_view_;

    std::weak_ptr<SplitDistanceManager> split_distance_manager_;
    AutoResetValue<bool> is_handling_split_distance_event_{};
};

}