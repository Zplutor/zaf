#include <format>
#include <zaf/application.h>
#include <zaf/base/log.h>
#include <zaf/graphic/dpi.h>
#include <zaf/control/button.h>
#include <zaf/window/window.h>
#include <zaf/rx/scheduler/main_thread_scheduler.h>
#include <zaf/rx/scheduler/new_thread_scheduler.h>
#include <zaf/rx/timer.h>
#include <zaf/control/layout/linear_layouter.h>
#include <zaf/window/screen_manager.h>
#include <zaf/control/text_box.h>
#include <zaf/window/tray_icon.h>
#include <zaf/window/popup_menu.h>

// {D5CD59A3-D13B-41F9-B74D-793C713F05DC}
static const GUID TrayIconID =
{ 0xd5cd59a3, 0xd13b, 0x41f9, { 0xb7, 0x4d, 0x79, 0x3c, 0x71, 0x3f, 0x5, 0xdc } };


void BeginRun(const zaf::ApplicationStartedInfo& event_info);

class Window : public zaf::Window {
protected:
    void Initialize() override {

        __super::Initialize();

        tray_icon_ = zaf::Create<zaf::TrayIcon>(TrayIconID);
        tray_icon_->SetTooltip(L"Playground");
        tray_icon_->Add();
        Disposables() += tray_icon_->ActivateEvent().Subscribe([this](const zaf::TrayIconActivateInfo& event_info) {
            ZAF_LOG() << L"Tray icon activated.";
        });
        Disposables() += tray_icon_->ContextMenuEvent().Subscribe([this](const zaf::TrayIconContextMenuInfo& event_info) {
            ZAF_LOG() << L"Tray icon context menu requested.";
            auto menu = zaf::Create<zaf::PopupMenu>();
            auto menu_item = zaf::Create<zaf::MenuItem>();
            menu_item->SetText(L"Menu Item 1");
            menu->AddMenuItem(menu_item);

            auto screen = zaf::ScreenManager::Instance().ScreenFromGlobalPosition(event_info.PositionInGlobal());
            auto position = screen->TransformFromGlobal(event_info.PositionInGlobal());
            menu->PopupOnScreen(
                screen,
                position, 
                zaf::PopupMenuOptions::AlignBottomLeft);
        });

        this->SetIsPopup(true);

        text_box_ = zaf::Create<zaf::TextBox>();
        text_box_->SetRect(zaf::Rect{ 0, 0, 200, 100 });
        text_box_->SetFontSize(14);
        text_box_->SetCaretColor(zaf::Color::Red());
        text_box_->SetText(L"TextBox");
        text_box_->SetBackgroundColor(zaf::Color::Black());
        text_box_->SetTextColor(zaf::Color::White());
        Disposables() += text_box_->TextChangedEvent().Subscribe([this](const zaf::TextChangedInfo& event_info) {
            ZAF_LOG() << L"TextChanged: " << text_box_->Text();
        });
        RootControl()->AddChild(text_box_);

        auto button = zaf::Create<zaf::Button>();
        button->SetRect(zaf::Rect{ 0, 100, 200, 30 });
        RootControl()->AddChild(button);
    }

    void OnShow(const zaf::ShowInfo& event_info) override {
        __super::OnShow(event_info);
        text_box_->SetIsFocused(true);
    }

private:
    std::shared_ptr<zaf::TextBox> text_box_;
    std::shared_ptr<zaf::TrayIcon> tray_icon_;
};


int WINAPI WinMain(
    HINSTANCE /* hInstance */,
    HINSTANCE /* hPrevInstance */,
    LPSTR /* lpCmdLine */,
    int /* nCmdShow */
) {

    SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

    auto& application = zaf::Application::Instance();
    application.Disposables() += application.StartedEvent().Subscribe(BeginRun);

    application.Initialize({});

    application.Run();
}


void BeginRun(const zaf::ApplicationStartedInfo& event_info) {

    auto window = zaf::Create<Window>();
    window->Show();
    zaf::Application::Instance().SetMainWindow(window);
}