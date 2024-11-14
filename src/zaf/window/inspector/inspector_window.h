#pragma once

#include <zaf/window/window.h>
#include <zaf/control/property_grid.h>
#include <zaf/control/split_control.h>
#include <zaf/control/tree_control_delegate.h>
#include <zaf/window/inspector/internal/inspector_port.h>

namespace zaf {
namespace internal {
class InspectDataSource;
}

class TreeControl;

class InspectorWindow : public Window, public TreeControlDelegate, public internal::InspectorPort {
public:
    InspectorWindow(const std::shared_ptr<Window>& target_window);
    
    float EstimateItemHeight(
        const std::shared_ptr<Object>& parent_item_data,
        std::size_t item_index,
        const std::shared_ptr<Object>& item_data) override;

    std::shared_ptr<TreeItem> CreateItem(
        const std::shared_ptr<Object>& parent_item_data,
        std::size_t item_index,
        const std::shared_ptr<Object>& item_data) override;

    void HighlightControl(const std::shared_ptr<Control>& control) override;
    void SelectControl(const std::shared_ptr<Control>& control) override;
    void ControlAddChild(const std::shared_ptr<Control>& parent, std::size_t added_index) override;
    void ControlRemoveChild(
        const std::shared_ptr<Control>& parent,
        std::size_t removed_index) override;

protected:
    void Initialize() override;

private:
    friend class Window;

    std::shared_ptr<internal::InspectorPort> GetPort();

private:
    void InitializeSplitControl();
    void InitializeFirstPaneToolbar();
    void InitializeTreeControl();
    void InitializeSecondPaneToolbar();
    void InitializePropertyGrid();
    void ChangeHighlightObject(const std::shared_ptr<Object>& object);

private:
    std::shared_ptr<SplitControl> split_control_;
    std::shared_ptr<TreeControl> tree_control_;
    std::shared_ptr<PropertyGrid> property_grid_;
    std::shared_ptr<internal::InspectDataSource> data_source_;
    std::shared_ptr<Window> target_window_;

    std::shared_ptr<Object> highlight_object_;
};

}