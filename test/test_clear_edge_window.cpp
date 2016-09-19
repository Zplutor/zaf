#include "test_clear_edge_window.h"
#include <zaf/creation.h>
#include <zaf/window/window.h>

class TestClearEdgeWindow : public zaf::Window {
public:
    void Initialize() override {

        __super::Initialize();

        SetTitle(L"Test clear edge");

        const int count = 3;
        float x = 0;
        float width = 100.0f / count;
        
        for (int index = 0; index < count; ++index) {

            auto control = zaf::Create<zaf::Control>();
            control->SetBackgroundColor(zaf::Color::White);
            control->SetBorderColor(zaf::Color::Black);
            control->SetBorderThickness(1);
            control->SetRect(zaf::Rect(x, 0, width, 20));
            GetRootControl()->AddChild(control);
            x += width;
        }
    }
};


void ShowTestClearEdgeWindow() {

    auto window = zaf::Create<TestClearEdgeWindow>();
    window->Show();
}