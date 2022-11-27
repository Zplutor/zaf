#include "main_window.h"
#include <zaf/control/layout/linear_layouter.h>
#include <zaf/creation.h>
#include <zaf/object/boxing/boxing.h>
#include "control_property_panel.h"
#include "control_view_panel.h"
#include "manager/button_explore_manager.h"
#include "manager/check_box_explore_manager.h"
#include "manager/combo_box_explore_manager.h"
#include "manager/label_explore_manager.h"
#include "manager/list_box_explore_manager.h"
#include "manager/radio_button_explore_manager.h"
#include "manager/scroll_bar_explore_manager.h"
#include "manager/split_control_explore_manager.h"
#include "manager/text_box_explore_manager.h"

static const struct {
    const wchar_t* name;
    std::function<std::shared_ptr<ExploreManager>()> createor;
} kControlNameAndCreators[] = {
    L"Button", []() { return std::make_shared<ButtonExploreManager>(); },
    L"CheckBox", []() { return std::make_shared<CheckBoxExploreManager>(); },
    L"ComboBox", []() { return std::make_shared<ComboBoxExploreManager>(); },
    //L"ImageBox", []() { return std::make_shared<ImageBoxExploreManager>(); },
    L"Label", []() { return std::make_shared<LabelExploreManager>(); },
    L"ListBox", []() { return std::make_shared<ListBoxExploreManager>(); },
    L"RadioButton", []() { return std::make_shared<RadioButtonExploreManager>(); },
    L"ScrollBar", []() { return std::make_shared<ScrollBarExploreManager>(); },
    L"SplitControl", []() { return std::make_shared<SplitControlExploreManager>(); },
    L"TextBox", []() { return std::make_shared<TextBoxExploreManager>(); },
};

static std::shared_ptr<ExploreManager> CreateExploreManager(const std::wstring& control_name);

void MainWindow::Initialize() {

    __super::Initialize();

    zaf::Size size(800, 600);
    SetSize(size);
    SetMinSize(size);

    auto root_control = RootControl();
    root_control->SetLayouter(zaf::Create<zaf::HorizontalLayouter>());

    primary_split_control_ = zaf::Create<zaf::SplitControl>();
    primary_split_control_->SetSplitDistance(200);
    primary_split_control_->SetFirstPaneMinLength(100);
    primary_split_control_->SetFirstPaneMaxLength(300);
    root_control->AddChild(primary_split_control_);

    InitializeControlListPanel();
    InitializeControlExplorePanel();
}


void MainWindow::InitializeControlListPanel() {

    control_list_box_ = zaf::Create<zaf::ListBox>();
    control_list_box_->SetBorder(zaf::Frame(0));
    control_list_box_->SetAllowHorizontalScroll(false);
    control_list_box_->SetAutoHideScrollBars(true);

    Subscriptions() += control_list_box_->SelectionChangeEvent().Subscribe(
        std::bind(&MainWindow::ControlListBoxSelectionChange, this));

    for (const auto& each_item : kControlNameAndCreators) {
        control_list_box_->AddItem(zaf::Box(each_item.name));
    }

    primary_split_control_->SetFirstPane(control_list_box_);
}


void MainWindow::InitializeControlExplorePanel() {

    control_view_panel_ = zaf::Create<ControlViewPanel>();
    control_property_panel_ = zaf::Create<ControlPropertyPanel>();

    secondly_split_control_ = zaf::Create<zaf::SplitControl>();
    secondly_split_control_->SetSplitDistance(200);
    secondly_split_control_->SetFirstPaneMinLength(200);
    secondly_split_control_->SetFirstPaneMaxLength(400);
    secondly_split_control_->SetIsHorizontalSplit(true);
    secondly_split_control_->SetFirstPane(control_view_panel_);
    secondly_split_control_->SetSecondPane(control_property_panel_);

    primary_split_control_->SetSecondPane(secondly_split_control_);
}


void MainWindow::ControlListBoxSelectionChange() {

    auto selected_data = control_list_box_->GetFirstSelectedItemData();
    if (!selected_data) {
        return;
    }

    auto selected_control_name = zaf::Unbox<std::wstring>(selected_data);
    if (!selected_control_name) {
        return;
    }

    auto iterator = explore_managers_.find(*selected_control_name);
    if (iterator == explore_managers_.end()) {

        auto control_explore_manager = CreateExploreManager(*selected_control_name);
        if (control_explore_manager != nullptr) {
            iterator = explore_managers_.insert(std::make_pair(*selected_control_name, control_explore_manager)).first;
        }
    }

    if (iterator != explore_managers_.end()) {
        LoadControlExploreManager(iterator->second);
    }
}


void MainWindow::LoadControlExploreManager(const std::shared_ptr<ExploreManager>& explore_manager) {

    control_view_panel_->SetExploredControl(explore_manager->GetExploredControl());
    control_property_panel_->SetPropertyItems(explore_manager->GetPropertyItems());
}


static std::shared_ptr<ExploreManager> CreateExploreManager(const std::wstring& control_name) {

    for (const auto& each_item : kControlNameAndCreators) {
        if (control_name == each_item.name) {
            if (each_item.createor != nullptr) {
                return each_item.createor();
            }
        }
    }

    return nullptr;
}