#include <zaf/zaf.h>

static void ShowMainWindow(zaf::Application&);
static std::shared_ptr<zaf::Control> CreateColorListControl();

struct ColorItem {
    const wchar_t* name;
    DWORD index;
};

#define COLOR_ITEM(item) { L#item, item }
ColorItem g_color_items[] = {

    COLOR_ITEM(COLOR_3DDKSHADOW),
    COLOR_ITEM(COLOR_3DFACE),
    COLOR_ITEM(COLOR_3DHIGHLIGHT),
    COLOR_ITEM(COLOR_3DHILIGHT),
    COLOR_ITEM(COLOR_3DLIGHT),
    COLOR_ITEM(COLOR_3DSHADOW),
    COLOR_ITEM(COLOR_ACTIVEBORDER),
    COLOR_ITEM(COLOR_ACTIVECAPTION),
    COLOR_ITEM(COLOR_APPWORKSPACE),
    COLOR_ITEM(COLOR_BACKGROUND),
    COLOR_ITEM(COLOR_BTNFACE),
    COLOR_ITEM(COLOR_BTNHIGHLIGHT),
    COLOR_ITEM(COLOR_BTNHILIGHT),
    COLOR_ITEM(COLOR_BTNSHADOW),
    COLOR_ITEM(COLOR_BTNTEXT),
    COLOR_ITEM(COLOR_CAPTIONTEXT),
    COLOR_ITEM(COLOR_DESKTOP),
    COLOR_ITEM(COLOR_GRADIENTACTIVECAPTION),
    COLOR_ITEM(COLOR_GRADIENTINACTIVECAPTION),
    COLOR_ITEM(COLOR_GRAYTEXT),
    COLOR_ITEM(COLOR_HIGHLIGHT),
    COLOR_ITEM(COLOR_HIGHLIGHTTEXT),
    COLOR_ITEM(COLOR_HOTLIGHT),
    COLOR_ITEM(COLOR_INACTIVEBORDER),
    COLOR_ITEM(COLOR_INACTIVECAPTION),
    COLOR_ITEM(COLOR_INACTIVECAPTIONTEXT),
    COLOR_ITEM(COLOR_INFOBK),
    COLOR_ITEM(COLOR_INFOTEXT),
    COLOR_ITEM(COLOR_MENU),
    COLOR_ITEM(COLOR_MENUHILIGHT),
    COLOR_ITEM(COLOR_MENUBAR),
    COLOR_ITEM(COLOR_MENUTEXT),
    COLOR_ITEM(COLOR_SCROLLBAR),
    COLOR_ITEM(COLOR_WINDOW),
    COLOR_ITEM(COLOR_WINDOWFRAME),
    COLOR_ITEM(COLOR_WINDOWTEXT),
};


int WINAPI WinMain(
    HINSTANCE /* hInstance */,
    HINSTANCE /* hPrevInstance */,
    LPSTR /* lpCmdLine */,
    int /* nCmdShow */
) {

    auto& application = zaf::Application::GetInstance();
    application.GetBeginRunEvent().AddListener(ShowMainWindow);
    application.Initialize();
    application.Run();
}


static void ShowMainWindow(zaf::Application&) {

    auto main_window = zaf::Create<zaf::Window>();
    main_window->SetTitle(L"SystemColorViewer");
    main_window->GetRootControl()->SetLayouter(zaf::GetVerticalArrayLayouter());
    main_window->GetRootControl()->AddChild(CreateColorListControl());
    main_window->Show();
}


static std::shared_ptr<zaf::Control> CreateColorListControl() {

    auto color_list_control = zaf::Create<zaf::ScrollableControl>();
    color_list_control->SetRect(zaf::Rect(0, 0, 500, 500));
    color_list_control->SetScrollAreaSize(zaf::Size(0, sizeof(g_color_items) / sizeof(ColorItem) * 25));
    color_list_control->SetAutoHideScrollBars(true);

    auto scrolled_control = color_list_control->GetScrolledControl();
    
    for (const auto& each_item : g_color_items) {

        auto label = zaf::Create<zaf::Label>();
        label->SetText(each_item.name);
        label->SetParagraphAlignment(zaf::ParagraphAlignment::Center);

        auto color_control = zaf::Create<zaf::Control>();
        DWORD color = GetSysColor(each_item.index);
        color_control->SetBackgroundColorPicker([color](const zaf::Control&) {
            return zaf::Color::FromCOLORREF(color);
        });

        auto item_control = zaf::Create<zaf::Control>();
        item_control->SetLayouter(zaf::GetHorizontalArrayLayouter());
        item_control->AddChild(label);
        item_control->AddChild(color_control);

        scrolled_control->AddChild(item_control);
    }

    scrolled_control->SetLayouter(zaf::GetVerticalArrayLayouter());

    return color_list_control;
}