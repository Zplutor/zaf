#include <zaf/window/popup_menu.h>
#include <zaf/base/container/utility/erase.h>
#include <zaf/base/log.h>
#include <zaf/control/layout/linear_layouter.h>
#include <zaf/creation.h>
#include <zaf/object/type_definition.h>

namespace zaf {

ZAF_DEFINE_TYPE(PopupMenu)
ZAF_DEFINE_TYPE_END


void PopupMenu::Initialize() {

    __super::Initialize();

    this->SetIsPopup(true);
    this->SetIsToolWindow(true);
    this->SetHasTitleBar(false);
    this->SetHasSystemMenu(false);
    this->SetIsSizable(false);
    this->SetInitialRectStyle(InitialRectStyle::Custom);
    this->SetActivateOption(ActivateOption::NoActivate);

    auto root_control = this->RootControl();
    root_control->SetLayouter(Create<VerticalLayouter>());
    root_control->SetBackgroundColor(zaf::Color::White());
}


void PopupMenu::OnShow(const ShowInfo& event_info) {

    __super::OnShow(event_info);

    //RootControl()->CaptureMouse();

    root_control_subscriptions_.Clear();

    root_control_subscriptions_ += RootControl()->MouseDownEvent().Subscribe(
        [this](const MouseDownInfo& event_info) {

        if (!ContentRect().Contain(event_info.PositionAtSender())) {
            Close();
        }
    });

    root_control_subscriptions_ += RootControl()->MouseReleasedEvent().Subscribe(
        [this](const MouseReleasedInfo& event_info) {

        if (!showing_sub_menu_item_.lock()) {
            Close();
        }
    });
}


void PopupMenu::AddMenuItem(const std::shared_ptr<MenuItem>& menu_item) {

    ZAF_EXPECT(menu_item);

    auto menu_item_info = std::make_unique<MenuItemInfo>();
    menu_item_info->menu_item = menu_item;
    InitializeMenuItem(*menu_item_info);

    menu_item_infos_.push_back(std::move(menu_item_info));
    RootControl()->AddChild(menu_item);

    //TODO: Update window height if menu is being shown.
}


void PopupMenu::InitializeMenuItem(MenuItemInfo& item_info) {

    auto& subscriptions = item_info.subscriptions;
    const auto& menu_item = item_info.menu_item;

    subscriptions += menu_item->MouseUpEvent().Subscribe(
        std::bind(&PopupMenu::OnMenuItemClick, this, std::placeholders::_1));

    subscriptions += menu_item->MouseHoverEvent().Subscribe(
        std::bind(&PopupMenu::OnMenuItemHover, this, std::placeholders::_1));

    subscriptions += menu_item->SubMenuShowEvent().Subscribe(
        std::bind(&PopupMenu::OnSubMenuShow, this, std::placeholders::_1));

    subscriptions += menu_item->SubMenuCloseEvent().Subscribe(
        std::bind(&PopupMenu::OnSubMenuClose, this, std::placeholders::_1));
}


void PopupMenu::OnMenuItemClick(const MouseUpInfo& event_info) {

    auto menu_item = As<MenuItem>(event_info.Sender());
    if (menu_item && !menu_item->HasSubMenuItem()) {
        this->Close();
        EnableWindow(Owner()->Handle(), TRUE);
    }
}


void PopupMenu::OnMenuItemHover(const MouseHoverInfo& event_info) {

    auto menu_item = As<MenuItem>(event_info.Sender());
    if (!menu_item || !menu_item->HasSubMenuItem()) {
        return;
    }

    showing_sub_menu_item_ = menu_item;
    menu_item->PopupSubMenu();
}


void PopupMenu::OnSubMenuShow(const SubMenuShowInfo& event_info) {

}


void PopupMenu::OnSubMenuClose(const SubMenuCloseInfo& event_info) {

}


void PopupMenu::RemoveMenuItem(const std::shared_ptr<MenuItem>& menu_item) {

    ZAF_EXPECT(menu_item);

    EraseIf(menu_item_infos_, [&menu_item](const auto& each_info) {
        return each_info->menu_item == menu_item;
    });

    RootControl()->RemoveChild(menu_item);

    //TODO: Update window height if menu is being shown.
}


void PopupMenu::Popup(const Point& position_in_screen) {

    this->SetPosition(position_in_screen);
    this->SetContentSize(CalculateMenuSize());

    this->Show();

    SetCapture(Owner()->Handle());

    ZAF_LOG() << "Menu handle: " << this->Handle();

    Subscriptions() += Owner()->MessageReceivedEvent().Subscribe([this](const MessageReceivedInfo& event_info) {
    
        auto id = event_info.Message().ID();

        if (id >= WM_MOUSEFIRST && id <= WM_MOUSELAST) {

            POINT mouse_position{};
            mouse_position.x = GET_X_LPARAM(event_info.Message().LParam());
            mouse_position.y = GET_Y_LPARAM(event_info.Message().LParam());

            ClientToScreen(Owner()->Handle(), &mouse_position);
            ScreenToClient(this->Handle(), &mouse_position);

            ZAF_LOG() << "X: " << mouse_position.x << ", Y: " << mouse_position.y;

            LRESULT result = SendMessage(
                this->Handle(),
                event_info.Message().ID(), 
                event_info.Message().WParam(), 
                MAKELPARAM(mouse_position.x, mouse_position.y));

            event_info.MarkAsHandled(result);
        }
    });
}


void PopupMenu::OnMessageReceived(const MessageReceivedInfo& event_info) {

    if (event_info.Message().ID() == WM_MOUSEMOVE) {

        MouseMessage mm{ event_info.Message() };
        auto mp = mm.MousePosition();

        ZAF_LOG() << "MenuX: " << mp.x << ", MenuY: " << mp.y;
    }

    __super::OnMessageReceived(event_info);
}


zaf::Size PopupMenu::CalculateMenuSize() const {

    zaf::Size result;

    for (const auto& each_child : RootControl()->Children()) {

        auto preferred_size = each_child->CalculatePreferredSize();
        result.width = std::max(result.width, preferred_size.width);

        //All menu items have set fixed height.
        result.height += each_child->Height();
    }

    //Width in float will be truncated when setting window width in integer, 
    //so we ceil the value here.
    //Note: this should be done within window itself.
    result.width = std::ceilf(result.width);

    result.width = std::max(result.width, 100.f);
    return result;
}


void PopupMenu::OnDestroyed(const DestroyedInfo& event_info) {

    __super::OnDestroyed(event_info);

    ReleaseCapture();
}

}