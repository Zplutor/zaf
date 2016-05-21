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

private:
    std::shared_ptr<ListBox> list_box_;
    std::shared_ptr<Control> options_container_;
};

void ShowTestListBoxWindow() {

    auto window = std::make_shared<TestListBoxWindow>();
    window->Initialize();
    window->Show();
}