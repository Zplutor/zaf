#include <gtest/gtest.h>
#include <zaf/application.h>
#include <zaf/creation.h>
#include <zaf/window/window.h>

TEST(ApplicationTest, MainWindowDestroyed) {

    //In this test case, check if there is runtime exception on main window destroyed.

    auto window = zaf::Create<zaf::Window>();
    window->SetInitialRectStyle(zaf::Window::InitialRectStyle::Custom);
    window->SetRect(zaf::Rect{ -100, -100, 100, 100 });
    window->Show();

    zaf::Application::Instance().SetMainWindow(window);

    window->Close();
}