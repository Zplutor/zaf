#include "utility/test_window.h"
#include <zaf/creation.h>

void TestWithWindow(const std::function<void(zaf::Window&)>& test) {

    auto window = zaf::Create<zaf::Window>();
    window->SetInitialRectStyle(zaf::InitialRectStyle::Custom);
    window->SetRect(zaf::Rect{ 0, 0, 100, 100 });

    window->Show();
    test(*window);
    window->Destroy();
}
