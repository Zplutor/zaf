#include "test_list_box_window.h"
#include <zaf/zaf.h>

using namespace zaf;

class TestListBoxWindow : public Window {
public:
    void Initialize() {

        SetTitle(L"test list box");
        SetRect(Rect(0, 0, 500, 500));

        list_box_ = CreateControl<ListBox>();
        list_box_->SetRect(Rect(0, 0, 200, 200));
        list_box_->AddItemWithText(L"Apple");
        list_box_->AddItemWithText(L"Boy");
        list_box_->AddItemWithText(L"Cat");
        list_box_->AddItemWithText(L"Dog");

        GetRootControl()->AddChild(list_box_);

        InitializeOptions();
    }


    void InitializeOptions() {

        options_container_ = CreateControl<Control>();
        options_container_->SetRect(Rect(0, 200, 500, 300));
        options_container_->SetLayouter(GetVerticalArrayLayouter());
        GetRootControl()->AddChild(options_container_);

        InitializeAddItemWithTextOptions();
        InitializeRemoveItemWithTextOptions();
        InitializeSelectOptionOptions();
        InitializeFirstSelectedIndexOptions();
    }


    void InitializeAddItemWithTextOptions() {

        auto container = CreateControl<Control>();
        container->SetLayouter(GetHorizontalArrayLayouter());
        options_container_->AddChild(container);
        
        auto text_box = CreateControl<TextBox>();
        container->AddChild(text_box);

        auto label = CreateControl<Label>();

        auto add_button = CreateControl<Button>();
        add_button->SetText(L"Add item");
        add_button->GetClickEvent().AddListener([this, text_box, label](const std::shared_ptr<ClickableControl>& button) {
            auto index = list_box_->AddItemWithText(text_box->GetText());
            label->SetText(std::to_wstring(index));
        });
        container->AddChild(add_button);
        container->AddChild(label);
    }


    void InitializeRemoveItemWithTextOptions() {

        auto container = CreateControl<Control>();
        container->SetLayouter(GetHorizontalArrayLayouter());
        options_container_->AddChild(container);

        auto text_box = CreateControl<TextBox>();
        container->AddChild(text_box);

        auto label = CreateControl<Label>();

        auto remove_button = CreateControl<Button>();
        remove_button->SetText(L"Remove item");
        remove_button->GetClickEvent().AddListener([this, text_box, label](const std::shared_ptr<ClickableControl>& button) {
            auto index = list_box_->RemoveItemWithText(text_box->GetText());
            label->SetText(std::to_wstring(index));
        });
        container->AddChild(remove_button);
        container->AddChild(label);
    }


    void InitializeSelectOptionOptions() {

        auto container = CreateControl<Control>();
        container->SetLayouter(GetHorizontalArrayLayouter());
        options_container_->AddChild(container);

        auto group = std::make_shared<RadioButton::Group>();

        auto single_select_radio_button = CreateControl<RadioButton>();
        single_select_radio_button->SetText(L"Single select");
        single_select_radio_button->SetGroup(group);
        single_select_radio_button->GetSelectStateChangeEvent().AddListener([this](const std::shared_ptr<RadioButton>&) {
            list_box_->SetSelectOption(ListBox::SelectOption::SingleSelect);
        });
        container->AddChild(single_select_radio_button);

        auto simple_multi_select_radio_button = CreateControl<RadioButton>();
        simple_multi_select_radio_button->SetText(L"Simple multi select");
        simple_multi_select_radio_button->SetGroup(group);
        simple_multi_select_radio_button->GetSelectStateChangeEvent().AddListener([this](const std::shared_ptr<RadioButton>&) {
            list_box_->SetSelectOption(ListBox::SelectOption::SimpleMultiSelect);
        });
        container->AddChild(simple_multi_select_radio_button);

        auto extended_multi_select_radio_button = CreateControl<RadioButton>();
        extended_multi_select_radio_button->SetText(L"Extended multi select");
        extended_multi_select_radio_button->SetGroup(group);
        extended_multi_select_radio_button->GetSelectStateChangeEvent().AddListener([this](const std::shared_ptr<RadioButton>&) {
            list_box_->SetSelectOption(ListBox::SelectOption::ExtendedMultiSelect);
        });
        container->AddChild(extended_multi_select_radio_button);

        switch (list_box_->GetSelectOption()) {
            case ListBox::SelectOption::SingleSelect:
                single_select_radio_button->SetSelected();
                break;
            case ListBox::SelectOption::SimpleMultiSelect:
                simple_multi_select_radio_button->SetSelected();
                break;
            case ListBox::SelectOption::ExtendedMultiSelect:
                simple_multi_select_radio_button->SetSelected();
                break;
        }
    }


    void InitializeFirstSelectedIndexOptions() {

        auto container = CreateControl<Control>();
        container->SetLayouter(GetHorizontalArrayLayouter());
        options_container_->AddChild(container);

        auto label = CreateControl<Label>();
        label->SetText(L"First selected index");
        container->AddChild(label);

        auto text_box = CreateControl<TextBox>();
        text_box->SetText(std::to_wstring(list_box_->GetFirstSelectedItemIndex()));
        container->AddChild(text_box);

        list_box_->GetSelectionChangeEvent().AddListener([text_box](const std::shared_ptr<ListBox>& list_box) {
            text_box->SetText(std::to_wstring(list_box->GetFirstSelectedItemIndex()));
        });
    }

private:
    std::shared_ptr<ListBox> list_box_;
    std::shared_ptr<Control> options_container_;
};

void ShowTestListBoxWindow() {

    auto window = std::make_shared<TestListBoxWindow>();
    window->Initialize();
    window->Show();
}