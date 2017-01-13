#include "main_window.h"
#include <zaf/control/layout/array_layouter.h>
#include <zaf/creation.h>
#include "control_operate_panel.h"
#include "control_view_panel.h"
#include "control/button/button_explore_manager.h"
#include "control/check_box/check_box_explore_manager.h"
#include "control/label/label_explore_manager.h"
#include "control/list_box/list_box_explore_manager.h"
#include "control/radio_button/radio_button_explore_manager.h"
#include "control/text_box/text_box_explore_manager.h"

static const struct {
    const wchar_t* name;
    std::function<std::shared_ptr<ControlExploreManager>()> createor;
} kControlNameAndCreators[] = {
    L"Button", []() { return std::make_shared<ButtonExploreManager>(); },
    L"CheckBox", []() { return std::make_shared<CheckBoxExploreManager>(); },
    L"Label", []() { return std::make_shared<LabelExploreManager>(); },
    L"ListBox", []() { return std::make_shared<ListBoxExploreManager>(); },
    L"RadioButton", []() { return std::make_shared<RadioButtonExploreManager>(); },
    L"TextBox", []() { return std::make_shared<TextBoxExploreManager>(); },
};

static std::shared_ptr<ControlExploreManager> CreateControlExploreManager(const std::wstring& control_name);

void MainWindow::Initialize() {

    __super::Initialize();

    auto root_control = GetRootControl();
    root_control->SetLayouter(zaf::GetHorizontalArrayLayouter());

    primary_split_control_ = zaf::Create<zaf::SplitControl>();
    root_control->AddChild(primary_split_control_);

    InitializeControlListPanel();
    InitializeControlExplorePanel();
}


void MainWindow::InitializeControlListPanel() {

    control_list_box_ = zaf::Create<zaf::ListBox>();
    control_list_box_->SetBorderThickness(0);
    control_list_box_->SetAllowHorizontalScroll(false);
    control_list_box_->SetAutoHideScrollBars(true);
    control_list_box_->GetSelectionChangeEvent().AddListener(std::bind(&MainWindow::ControlListBoxSelectionChange, this));

    for (const auto& each_item : kControlNameAndCreators) {
        control_list_box_->AddItemWithText(each_item.name);
    }

    primary_split_control_->SetFirstPane(control_list_box_);
}


void MainWindow::InitializeControlExplorePanel() {

    control_view_panel_ = zaf::Create<ControlViewPanel>();
    control_operate_panel_ = zaf::Create<ControlOperatePanel>();

    secondly_split_control_ = zaf::Create<zaf::SplitControl>();
    secondly_split_control_->SetIsHorizontal(true);
    secondly_split_control_->SetFirstPane(control_view_panel_);
    secondly_split_control_->SetSecondPane(control_operate_panel_);

    primary_split_control_->SetSecondPane(secondly_split_control_);
}


void MainWindow::WindowCreate() {

    __super::WindowCreate();

    primary_split_control_->SetSplitBarDistance(200);
    secondly_split_control_->SetSplitBarDistance(200);
}


void MainWindow::ControlListBoxSelectionChange() {

    auto selected_text = control_list_box_->GetFirstSelectedItemText();

    auto iterator = control_explore_managers_.find(selected_text);
    if (iterator == control_explore_managers_.end()) {

        auto control_explore_manager = CreateControlExploreManager(selected_text);
        if (control_explore_manager != nullptr) {
            control_explore_manager->Initialize();
            iterator = control_explore_managers_.insert(std::make_pair(selected_text, control_explore_manager)).first;
        }
    }

    if (iterator != control_explore_managers_.end()) {
        LoadControlExploreManager(iterator->second);
    }
}


void MainWindow::LoadControlExploreManager(const std::shared_ptr<ControlExploreManager>& control_explore_manager) {

    control_view_panel_->RemoveAllChildren();
    control_view_panel_->AddChild(control_explore_manager->GetControl());

    control_operate_panel_->SetOperatePanels(control_explore_manager->GetOperatePanels());
}


static std::shared_ptr<ControlExploreManager> CreateControlExploreManager(const std::wstring& control_name) {

    for (const auto& each_item : kControlNameAndCreators) {
        if (control_name == each_item.name) {
            if (each_item.createor != nullptr) {
                return each_item.createor();
            }
        }
    }

    return nullptr;
}