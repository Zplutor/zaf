#pragma once

#include <zaf/window/window.h>

namespace zaf {

class PopupMenu : public Window {
public:

protected:
    void Initialize() override;

    void OnHandleCreated(const HandleCreatedInfo& event_info) override;
    void OnShow(const ShowInfo& event_info) override;
};

}