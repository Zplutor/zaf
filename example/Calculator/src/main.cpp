#include <Windows.h>
#include <cwchar>
#include <zaf/zaf.h>
#include <zaf/base/string/to_string.h>
#include "logic/calculate_result.h"
#include "logic/calculator.h"

static void Initialize(zaf::Application& application);

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

    auto& application = zaf::Application::GetInstance();
    application.GetBeginRunEvent().AddListener(Initialize);

    std::error_code error_code;
    application.Initialize(error_code);

    if (zaf::IsSucceeded(error_code)) {
        application.Run();
    }
}


static void Initialize(zaf::Application& application) {

    auto main_window = zaf::Create<zaf::Dialog>();
    main_window->SetInitialRectStyle(zaf::Window::InitialRectStyle::CenterInScreen);
    main_window->SetSize(zaf::Size(300, 200));

    auto input_text_box = zaf::Create<zaf::TextBox>();
    input_text_box->SetRect(zaf::Rect(0, 0, 150, 30));
    input_text_box->SetTextAlignment(zaf::TextAlignment::Center);
    input_text_box->SetParagraphAlignment(zaf::ParagraphAlignment::Center);
    main_window->GetRootControl()->AddChild(input_text_box);

    auto output_label = zaf::Create<zaf::Label>();
    output_label->SetRect(zaf::Rect(0, 40, 150, 30));
    main_window->GetRootControl()->AddChild(output_label);

    auto calculate_button = zaf::Create<zaf::Button>();
    calculate_button->SetRect(zaf::Rect(160, 0, 50, 30));
    calculate_button->SetText(L"C");
    calculate_button->GetClickEvent().AddListener([input_text_box, output_label](const std::shared_ptr<zaf::ClickableControl>& button) {

        Calculator calculator;
        auto result = calculator.Calculate(input_text_box->GetText());
        if (result->error == CalculateResult::Error::None) {
            output_label->SetText(result->text);
        }
    });
    main_window->GetRootControl()->AddChild(calculate_button);
    main_window->SetDefaultButton(calculate_button);

    application.SetMainWindow(main_window);
    main_window->Show();
    input_text_box->SetIsFocused(true);
}