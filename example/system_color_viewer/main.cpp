#include <zaf/application.h>
#include <zaf/control/label.h>
#include <zaf/control/layout/linear_layouter.h>
#include <zaf/control/scrollable_control.h>
#include <zaf/creation.h>
#include <zaf/window/window.h>

static void ShowMainWindow(const zaf::ApplicationBeginRunInfo&);
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


int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

    auto& application = zaf::Application::Instance();
    application.Subscriptions() += application.BeginRunEvent().Subscribe(ShowMainWindow);

    application.Initialize({});
    application.Run();
}


static void ShowMainWindow(const zaf::ApplicationBeginRunInfo&) {

    auto main_window = zaf::Create<zaf::Window>();
    main_window->SetTitle(L"SystemColorViewer");

    auto root_control = main_window->RootControl();
    root_control->AddChild(CreateColorListControl());

    //Setting a linear layouter to root control can fill the window 
    //client area with color list control automatically.
    root_control->SetLayouter(zaf::Create<zaf::VerticalLayouter>());

    zaf::Application::Instance().SetMainWindow(main_window);
    main_window->Show();
}


static std::shared_ptr<zaf::Control> CreateColorListControl() {

    const float item_height = 25;

    auto color_list_control = zaf::Create<zaf::ScrollableControl>();
    color_list_control->SetBorder(zaf::Frame(0, 1, 0, 0));
    color_list_control->SetAutoHideScrollBars(true);

    //The width of scroll content size is unconcerned here, so just set it to 0.
    //The scrollable control will adjust the size to a appropriate value.
    color_list_control->SetScrollContentSize(zaf::Size(0, sizeof(g_color_items) / sizeof(ColorItem) * item_height));

    auto scrolled_content_control = color_list_control->GetScrollContentControl();
    scrolled_content_control->SetLayouter(zaf::Create<zaf::VerticalLayouter>());
    
    //Begin update the control, to avoid relayouting frequently when adding children. 
    auto update_guard = scrolled_content_control->BeginUpdate();
    
    for (const auto& each_item : g_color_items) {

        auto label = zaf::Create<zaf::Label>();
        label->SetText(each_item.name);
        label->SetParagraphAlignment(zaf::ParagraphAlignment::Center);

        auto color_control = zaf::Create<zaf::Control>();
        COLORREF color_rgb = GetSysColor(each_item.index);
        color_control->SetBackgroundColor(zaf::Color::FromCOLORREF(color_rgb));

        auto item_control = zaf::Create<zaf::Control>();
        item_control->SetLayouter(zaf::Create<zaf::HorizontalLayouter>());
        item_control->SetPadding(zaf::Frame(5, 0, 0, 0));
        item_control->AddChild(label);
        item_control->AddChild(color_control);

        scrolled_content_control->AddChild(item_control);
    }

    return color_list_control;
}