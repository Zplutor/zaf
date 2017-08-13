#include "main_window.h"
#include <zaf/control/layout/array_layouter.h>
#include <zaf/creation.h>
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
    SetMinimumSize(size);

    auto root_control = GetRootControl();
    root_control->SetLayouter(zaf::GetHorizontalArrayLayouter());

    primary_split_control_ = zaf::Create<zaf::SplitControl>();
    primary_split_control_->SetSplitBarDistance(200);
    primary_split_control_->SetMinimumSplitBarDistance(100);
    primary_split_control_->SetMaximumSplitBarDistance(300);
    root_control->AddChild(primary_split_control_);

    InitializeControlListPanel();
    InitializeControlExplorePanel();
}


void MainWindow::InitializeControlListPanel() {

    control_list_box_ = zaf::Create<zaf::ListBox>();
    control_list_box_->SetBorder(0);
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
    control_property_panel_ = zaf::Create<ControlPropertyPanel>();

    secondly_split_control_ = zaf::Create<zaf::SplitControl>();
    secondly_split_control_->SetSplitBarDistance(200);
    secondly_split_control_->SetMinimumSplitBarDistance(200);
    secondly_split_control_->SetMaximumSplitBarDistance(400);
    secondly_split_control_->SetIsHorizontalSplit(true);
    secondly_split_control_->SetFirstPane(control_view_panel_);
    secondly_split_control_->SetSecondPane(control_property_panel_);

    primary_split_control_->SetSecondPane(secondly_split_control_);
}


void MainWindow::ControlListBoxSelectionChange() {

    auto selected_text = control_list_box_->GetFirstSelectedItemText();

    auto iterator = explore_managers_.find(selected_text);
    if (iterator == explore_managers_.end()) {

        auto control_explore_manager = CreateExploreManager(selected_text);
        if (control_explore_manager != nullptr) {
            iterator = explore_managers_.insert(std::make_pair(selected_text, control_explore_manager)).first;
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