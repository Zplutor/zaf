#include "test_window_window.h"
#include <zaf/control/button.h>
#include <zaf/control/check_box.h>
#include <zaf/control/layout/array_layouter.h>
#include <zaf/control/radio_button.h>
#include <zaf/creation.h>
#include <zaf/window/dialog.h>
#include <zaf/window/window.h>

using namespace zaf;

static std::shared_ptr<Button> CreateShowDialogButton(const std::shared_ptr<Window>& window);
static void ShowTestAppearenceWindow(const std::shared_ptr<Window>& owner);

void ShowTestWindowWindow() {

    auto window = Create<Window>();
    window->SetTitle(L"test window");
    window->GetRootControl()->SetLayouter(GetVerticalArrayLayouter());
    window->GetRootControl()->AddChild(CreateShowDialogButton(window));

    auto test_appearence_button = Create<Button>();
    test_appearence_button->SetText(L"Test appearence");
    test_appearence_button->GetClickEvent().AddListener([window](const std::shared_ptr<ClickableControl>&) {
        ShowTestAppearenceWindow(window);
    });
    window->GetRootControl()->AddChild(test_appearence_button);

    window->Show();
}


static std::shared_ptr<Button> CreateShowDialogButton(const std::shared_ptr<Window>& window) {

    auto show_dialog_button = Create<Button>();
    show_dialog_button->SetText(L"Show dialog");
    show_dialog_button->GetClickEvent().AddListener([window](const std::shared_ptr<ClickableControl>&) {

        auto dialog = Create<Dialog>();
        dialog->SetOwner(window);

        dialog->GetRootControl()->SetLayouter(GetHorizontalArrayLayouter());

        auto ok_button = Create<Button>();
        ok_button->SetText(L"OK");
        dialog->GetRootControl()->AddChild(ok_button);
        dialog->AddDialogButton(ok_button, DialogResult::Ok);
        dialog->SetDefaultButton(ok_button);

        auto cancel_button = Create<Button>();
        cancel_button->SetText(L"Cancel");
        dialog->GetRootControl()->AddChild(cancel_button);
        dialog->AddDialogButton(cancel_button, DialogResult::Cancel);

        dialog->ShowModally();
        auto result = dialog->GetDialogResult();
    });

    return show_dialog_button;
}


static void ShowTestAppearenceWindow(const std::shared_ptr<Window>& owner) {

    auto control_window = Create<Window>();
    control_window->SetTitle(L"Test appearence");
    control_window->GetRootControl()->SetLayouter(GetVerticalArrayLayouter());

    auto show_window = Create<Window>();
    show_window->SetOwner(control_window);
    show_window->SetTitle(L"Window");
    show_window->SetSize(Size(200, 200));
    show_window->GetRootControl()->SetBackgroundColor(Color::White);

    auto is_sizable_check_box = Create<CheckBox>();
    is_sizable_check_box->SetText(L"Is sizable");
    is_sizable_check_box->SetIsChecked(show_window->IsSizable());
    is_sizable_check_box->GetCheckStateChangeEvent().AddListener([show_window](const std::shared_ptr<CheckBox>& check_box) {
        show_window->SetIsSizable(check_box->IsChecked());
        check_box->SetIsChecked(show_window->IsSizable());
    });

    auto has_system_menu_check_box = Create<CheckBox>();
    has_system_menu_check_box->SetText(L"Has system menu");
    has_system_menu_check_box->SetIsChecked(show_window->HasSystemMenu());
    has_system_menu_check_box->GetCheckStateChangeEvent().AddListener([show_window](const std::shared_ptr<CheckBox>& check_box) {
        show_window->SetHasSystemMenu(check_box->IsChecked());
        check_box->SetIsChecked(show_window->HasSystemMenu());
    });

    auto has_maximize_button_check_box = Create<CheckBox>();
    has_maximize_button_check_box->SetText(L"Has maximize button");
    has_maximize_button_check_box->SetIsChecked(show_window->HasMaximizeButton());
    has_maximize_button_check_box->GetCheckStateChangeEvent().AddListener([show_window](const std::shared_ptr<CheckBox>& check_box) {
        show_window->SetHasMaximizeButton(check_box->IsChecked());
        check_box->SetIsChecked(show_window->HasMaximizeButton());
    });

    auto has_minimize_button_check_box = Create<CheckBox>();
    has_minimize_button_check_box->SetText(L"Has minimize button");
    has_minimize_button_check_box->SetIsChecked(show_window->HasMinimizeButton());
    has_minimize_button_check_box->GetCheckStateChangeEvent().AddListener([show_window](const std::shared_ptr<CheckBox>& check_box) {
        show_window->SetHasMinimizeButton(check_box->IsChecked());
        check_box->SetIsChecked(show_window->HasMinimizeButton());
    });

    //Border style
    auto border_style_container = Create<Control>();
    border_style_container->SetLayouter(GetHorizontalArrayLayouter());

    auto border_style_group = std::make_shared<RadioButton::Group>();

    auto change_border_style = [
            show_window,
            is_sizable_check_box,
            has_system_menu_check_box,
            has_maximize_button_check_box,
            has_minimize_button_check_box](Window::BorderStyle border_style) {

        show_window->Close();
        show_window->SetBorderStyle(border_style);
        show_window->Show();

        is_sizable_check_box->SetIsChecked(show_window->IsSizable());
        has_system_menu_check_box->SetIsChecked(show_window->HasSystemMenu());
        has_maximize_button_check_box->SetIsChecked(show_window->HasMaximizeButton());
        has_minimize_button_check_box->SetIsChecked(show_window->HasMinimizeButton());
    };

    auto none_border_style_radio_button = Create<RadioButton>();
    none_border_style_radio_button->SetText(L"None");
    none_border_style_radio_button->SetGroup(border_style_group);
    none_border_style_radio_button->GetSelectStateChangeEvent().AddListener([change_border_style](const std::shared_ptr<RadioButton>&) {
        change_border_style(Window::BorderStyle::None);
    });
    border_style_container->AddChild(none_border_style_radio_button);

    auto normal_border_style_radio_button = Create<RadioButton>();
    normal_border_style_radio_button->SetText(L"Normal");
    normal_border_style_radio_button->SetGroup(border_style_group);
    normal_border_style_radio_button->GetSelectStateChangeEvent().AddListener([change_border_style](const std::shared_ptr<RadioButton>&) {
        change_border_style(Window::BorderStyle::Normal);
    });
    border_style_container->AddChild(normal_border_style_radio_button);

    auto dialog_border_style_radio_button = Create<RadioButton>();
    dialog_border_style_radio_button->SetText(L"Dialog");
    dialog_border_style_radio_button->SetGroup(border_style_group);
    dialog_border_style_radio_button->GetSelectStateChangeEvent().AddListener([change_border_style](const std::shared_ptr<RadioButton>&) {
        change_border_style(Window::BorderStyle::Dialog);
    });
    border_style_container->AddChild(dialog_border_style_radio_button);

    auto tool_window_border_style_radio_button = Create<RadioButton>();
    tool_window_border_style_radio_button->SetText(L"Tool window");
    tool_window_border_style_radio_button->SetGroup(border_style_group);
    tool_window_border_style_radio_button->GetSelectStateChangeEvent().AddListener([change_border_style](const std::shared_ptr<RadioButton>&) {
        change_border_style(Window::BorderStyle::ToolWindow);
    });
    border_style_container->AddChild(tool_window_border_style_radio_button);
    
    control_window->GetRootControl()->AddChild(border_style_container);
    control_window->GetRootControl()->AddChild(is_sizable_check_box);
    control_window->GetRootControl()->AddChild(has_system_menu_check_box);
    control_window->GetRootControl()->AddChild(has_maximize_button_check_box);
    control_window->GetRootControl()->AddChild(has_minimize_button_check_box);
    control_window->Show();

    normal_border_style_radio_button->SetSelected();
}