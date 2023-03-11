#include <zaf/window/popup_menu.h>

namespace zaf {

void PopupMenu::Initialize() {

    __super::Initialize();

    this->SetIsPopup(true);
    this->SetIsToolWindow(true);
    this->SetHasTitleBar(false);
    this->SetHasSystemMenu(false);
    this->SetIsSizable(false);
    this->SetInitialRectStyle(InitialRectStyle::Custom);
    this->SetActivateOption(ActivateOption::NoActivate);
}


void PopupMenu::Popup(const Point& position_in_screen) {

    this->SetPosition(position_in_screen);
    this->Show();
}


void PopupMenu::OnHandleCreated(const HandleCreatedInfo& event_info) {

    __super::OnHandleCreated(event_info);

    auto owner = Owner();
    if (!owner) {
        return;
    }
}


void PopupMenu::OnShow(const ShowInfo& event_info) {

    __super::OnShow(event_info);

    RootControl()->CaptureMouse();

    root_control_subscriptions_ += RootControl()->MouseDownEvent().Subscribe(
        [this](const MouseDownInfo& event_info) {
    
        if (!ContentRect().Contain(event_info.PositionAtSender())) {
            Close();
        }
    });

    root_control_subscriptions_ += RootControl()->MouseReleasedEvent().Subscribe(
        [this](const MouseReleasedInfo& event_info) {
    
        Close();
    });
}

}