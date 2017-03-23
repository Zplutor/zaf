#pragma once

#include <zaf/window/window.h>
#include "ui/main/content_panel.h"

class MainWindow : public zaf::Window {
public:
    void Initialize() override;

private:
    std::shared_ptr<zaf::Control> content_panel_container_;
    std::shared_ptr<ContentPanel> conversation_panel_;
};