#pragma once

#include <zaf/window/window.h>
#include "conversation_list_view.h"

class MainWindow : public zaf::Window {
public:
    void Initialize() override;

private:
    std::shared_ptr<ConversationListView> conversation_list_view_;
};