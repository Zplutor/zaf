#include "test_combo_box_window.h"
#include <zaf/control/check_box.h>
#include <zaf/control/combo_box.h>
#include <zaf/control/layout/array_layouter.h>
#include <zaf/control/list_box.h>
#include <zaf/creation.h>
#include <zaf/window/window.h>

using namespace zaf;

class TestComboBoxWindow : public Window {
public:
    void Initialize() override {

        __super::Initialize();

        SetTitle(L"test combo box");

        auto root_control = GetRootControl();
        root_control->SetLayouter(GetHorizontalArrayLayouter());

        InitializeComboBox();
        InitializeOptions();
    }

    void InitializeComboBox() {

        auto container = Create<Control>();
        GetRootControl()->AddChild(container);

        combo_box_ = Create<ComboBox>();
        combo_box_->SetRect(Rect(0, 0, 100, 30));
        combo_box_->SetMaximumVisibleItemCount(5);
        container->AddChild(combo_box_);

        auto drop_down_list_box = combo_box_->GetDropDownListBox();
        for (std::size_t index = 0; index < 10; ++index) {
            drop_down_list_box->AddItemWithText(std::to_wstring(index));
        }
    }

    void InitializeOptions() {

        option_container_ = Create<Control>();
        option_container_->SetLayouter(GetVerticalArrayLayouter());
        GetRootControl()->AddChild(option_container_);

        InitializeIsEditableOption();
    }

    void InitializeIsEditableOption() {

        auto check_box = Create<CheckBox>();
        check_box->SetText(L"Is editable");
        check_box->SetIsChecked(combo_box_->IsEditable());
        check_box->GetCheckStateChangeEvent().AddListener([this](const std::shared_ptr<CheckBox>& check_box){
            combo_box_->SetIsEditable(check_box->IsChecked());
        });

        option_container_->AddChild(check_box);
    }

private:
    std::shared_ptr<ComboBox> combo_box_;
    std::shared_ptr<Control> option_container_;
};


void ShowTestComboBoxWindow() {

    auto window = Create<TestComboBoxWindow>();
    window->Show();
}