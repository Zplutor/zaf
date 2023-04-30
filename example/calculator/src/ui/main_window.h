#pragma once

#include <zaf/control/clickable_control.h>
#include <zaf/control/rich_edit.h>
#include <zaf/window/window.h>

class MainWindow : public zaf::Window {
public:
    void Initialize() override;

protected:
    void OnHandleCreated(const zaf::HandleCreatedInfo& event_info) override;

private:
    void InitializeRichEdits();
    void InitializeButtons();
    float GetContentWidth() const;
    void ButtonClick(const zaf::ClickInfo& event_info);

private:
    std::shared_ptr<zaf::RichEdit> input_rich_edit_;
    std::shared_ptr<zaf::RichEdit> output_rich_edit_;
};