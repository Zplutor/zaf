#include <zaf/window/popup_menu.h>

namespace zaf {

void PopupMenu::Initialize() {

    __super::Initialize();

    this->SetIsPopup(true);
    this->SetIsToolWindow(true);
    this->SetHasTitleBar(false);
    this->SetHasSystemMenu(false);
    this->SetIsSizable(false);
    this->SetActivateOption(ActivateOption::NoActivate);
}


void PopupMenu::OnHandleCreated(const HandleCreatedInfo& event_info) {

    __super::OnHandleCreated(event_info);

    auto owner = Owner();
    if (owner) {
    }
}


void PopupMenu::OnShow(const ShowInfo& event_info) {

    __super::OnShow(event_info);

    RootControl()->CaptureMouse();

    Subscriptions() += RootControl()->MouseDownEvent().Subscribe([this](const MouseDownInfo& event_info) {
    
        if (!ContentRect().Contain(event_info.PositionAtSender())) {
            Close();
        }
    });
}

}