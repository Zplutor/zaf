#pragma once

#include <zaf/control/clickable_control.h>
#include <zaf/control/text_box.h>
#include <zaf/window/window.h>

class MainWindow : public zaf::Window {
public:
    void Initialize() override;

protected:
    void WindowCreate() override;

private:
    void InitializeTextBoxes();
    void InitializeButtons();
    float GetContentWidth() const;
    void ButtonClick(const std::shared_ptr<zaf::ClickableControl>& button);

private:
    std::shared_ptr<zaf::TextBox> input_text_box_;
    std::shared_ptr<zaf::TextBox> output_text_box_;
};