#include "test_window_window.h"
#include <zaf/zaf.h>

using namespace zaf;

void ShowTestWindowWindow() {

    auto window = Create<Window>();
    window->SetTitle(L"test window");

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

    window->GetRootControl()->SetLayouter(GetVerticalArrayLayouter());
    window->GetRootControl()->AddChild(show_dialog_button);

    window->Show();
}