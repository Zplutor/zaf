#pragma once

#include <zaf/window/window.h>

namespace zaf {

class Label;

class TooltipWindow : public Window {
public:
    void ShowBelowMouseCursor();

    void SetText(const std::wstring& text);

protected:
    void Initialize() override;
    void OnWindowCreated() override;

private:
    std::shared_ptr<Label> label_;
};

}