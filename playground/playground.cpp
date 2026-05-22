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
#include <zaf/control/list_box.h>
#include <zaf/graphic/canvas.h>
#include <zaf/graphic/graphic_factory.h>
#include <zaf/graphic/d2d/stroke_properties.h>

void BeginRun(const zaf::ApplicationStartedInfo& event_info);

class CustomPaintControl : public zaf::Control {
protected:
    void Paint(zaf::Canvas& canvas, const zaf::Rect& dirty_rect) const override {

        canvas.DrawRectangle(this->RectInSelf(), zaf::Color::White());

        zaf::Rect region_rect{ 0.25, 0.25, 100, 100 };
        auto region = canvas.PushRegion(region_rect, region_rect);

        canvas.DrawRectangle(zaf::Rect{ 0, 0, 100, 100 }, zaf::Color::Gray());

        canvas.SetBrushWithColor(zaf::Color::Red());

        zaf::d2d::StrokeProperties stroke_properties;
        stroke_properties.SetStartCapStyle(zaf::d2d::Stroke::CapStyle::Square);
        auto stroke = zaf::GraphicFactory::Instance().CreateStroke(stroke_properties);
        canvas.SetStroke(stroke);
        canvas.DrawLine({ 0.25, 0.25 }, { 100.25, 0.25 }, 1);

        /*
        canvas.DrawRectangle(this->RectInSelf(), zaf::Color::White());

        constexpr float StrokeWidth = 15;

        zaf::Point start_point{ 50, 50 };
        zaf::Point middle_point{ start_point.x + 100, start_point.y };
        zaf::Point end_point{ middle_point.x, middle_point.y + 100 };

        canvas.SetBrushWithColor(zaf::Color::Green());
        canvas.DrawLine(start_point, middle_point, StrokeWidth);
        canvas.DrawLine(middle_point, end_point, StrokeWidth);

        zaf::Ellipse ellipse;
        ellipse.position.x = middle_point.x;
        ellipse.position.y = middle_point.y;
        ellipse.x_radius = StrokeWidth / 2;
        ellipse.y_radius = StrokeWidth / 2;
        canvas.DrawEllipse(ellipse);
        */
    }
};


class Window : public zaf::Window {
protected:
    void Initialize() override {

        __super::Initialize();

        this->SetRootControl(zaf::Create<CustomPaintControl>());
    }

private:
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
    window->SetTitle(L"New Canvas");
    window->Show();
    zaf::Application::Instance().SetMainWindow(window);
}