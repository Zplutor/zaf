#include "ui/main_window.h"
#include <zaf/base/as.h>
#include <zaf/control/button.h>
#include <zaf/creation.h>
#include <zaf/graphic/font.h>
#include "logic/calculate_result.h"
#include "logic/calculator.h"

static const float kContentMargin = 10;

void MainWindow::Initialize() {

    __super::Initialize();

    SetTitle(L"Calculator");
    SetIsSizable(false);
    SetCanMaximize(false);
 
    SetSize(zaf::Size(290, 320));
}


void MainWindow::OnHandleCreated(const zaf::HandleCreatedInfo& event_info) {

    __super::OnHandleCreated(event_info);

    InitializeRichEdits();
    InitializeButtons();
}


void MainWindow::InitializeRichEdits() {

    input_rich_edit_ = zaf::Create<zaf::RichEdit>();
    input_rich_edit_->SetRect(zaf::Rect(0, 0, GetContentWidth(), 30));
    input_rich_edit_->SetIsReadOnly(true);
    input_rich_edit_->SetBackgroundColor(zaf::Color::White());
    input_rich_edit_->SetBorder(zaf::Frame(0));
    input_rich_edit_->SetInset(50);

    output_rich_edit_ = zaf::Create<zaf::RichEdit>();
    output_rich_edit_->SetRect(zaf::Rect(0, 30, GetContentWidth(), 50));
    output_rich_edit_->SetIsReadOnly(true);
    output_rich_edit_->SetBackgroundColor(zaf::Color::White());
    output_rich_edit_->SetBorder(zaf::Frame(0));
    output_rich_edit_->SetInset(50);
    zaf::Font font;
    font.size = 36;
    output_rich_edit_->SetFont(font);

    auto container = zaf::Create<zaf::Control>();
    container->SetRect(zaf::Rect(kContentMargin, kContentMargin, GetContentWidth(), 80));
    container->SetBorder(zaf::Frame(1));
    container->SetBorderColor(zaf::Color::FromRGB(0x808080));
    container->AddChild(input_rich_edit_);
    container->AddChild(output_rich_edit_);

    RootControl()->AddChild(container);
}


void MainWindow::InitializeButtons() {

    const wchar_t* button_text[][5] = {
    
        { L"7", L"8", L"9", L"/", L"¡û" },
        { L"4", L"5", L"6", L"*", L"C" },
        { L"1", L"2", L"3", L"-", L"=" },
        { L"0", L"(", L")", L"+", nullptr },
    };
    
    const float button_spacing = 10;
    const float button_width = (GetContentWidth() - 10 * 4) / 5;
    const float button_height = 35;

    float x = kContentMargin;
    float y = 100;

    for (const auto& each_row : button_text) {

        for (auto each_text : each_row) {

            if (each_text != nullptr) {

                auto button = zaf::Create<zaf::Button>();
                button->SetText(each_text);

                float height = button_height;
                if (std::wcscmp(each_text, L"=") == 0) {
                    height += button_height + button_spacing;
                }

                button->SetRect(zaf::Rect(x, y, button_width, height));

                Subscriptions() += button->ClickEvent().Subscribe(
                    std::bind(&MainWindow::ButtonClick, this, std::placeholders::_1));

                RootControl()->AddChild(button);
            }

            x += button_width + button_spacing;
        }

        x = kContentMargin;
        y += button_height + button_spacing;
    }
}


float MainWindow::GetContentWidth() const {
    return ContentSize().width - kContentMargin * 2;
}


void MainWindow::ButtonClick(const zaf::ClickInfo& event_info) {

    auto button = zaf::As<zaf::Button>(event_info.Source());
    auto button_text = button->Text();
    if (button_text == L"=") {

        auto result = Calculator().Calculate(input_rich_edit_->Text());
        if (result->error == CalculateResult::Error::None) {
            output_rich_edit_->SetText(result->text);
        }
        else {
            output_rich_edit_->SetText(L"ERROR");
        }
    }
    else if (button_text == L"C") {
        input_rich_edit_->SetText(L"");
        output_rich_edit_->SetText(L"");
    }
    else if (button_text == L"¡û") {
        auto current_text = input_rich_edit_->Text();
        input_rich_edit_->SetText(current_text.substr(0, current_text.length() - 1));
    }
    else {
        
        input_rich_edit_->SetText(input_rich_edit_->Text() + button_text);
        input_rich_edit_->ScrollRightToEnd();
    }
}