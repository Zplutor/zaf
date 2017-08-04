#include <zaf/window/window.h>
#include <zaf/control/list_box.h>
#include <zaf/control/split_control.h>

class ControlPropertyPanel;
class ControlViewPanel;
class ExploreManager;

class MainWindow : public zaf::Window {
public:
    void Initialize() override;

private:
    void InitializeControlListPanel();
    void InitializeControlExplorePanel();

    void ControlListBoxSelectionChange();
    void LoadControlExploreManager(const std::shared_ptr<ExploreManager>& explore_manager);

private:
    std::shared_ptr<zaf::SplitControl> primary_split_control_;
    std::shared_ptr<zaf::SplitControl> secondly_split_control_;
    std::shared_ptr<zaf::ListBox> control_list_box_;
    std::shared_ptr<ControlViewPanel> control_view_panel_;
    std::shared_ptr<ControlPropertyPanel> control_property_panel_;
    std::map<std::wstring, std::shared_ptr<ExploreManager>> explore_managers_;
};