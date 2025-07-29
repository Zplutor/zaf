#include <gtest/gtest.h>
#include <zaf/application.h>
#include <zaf/creation.h>
#include <zaf/window/window.h>

static_assert(!std::is_copy_constructible_v<zaf::Application>);
static_assert(!std::is_copy_assignable_v<zaf::Application>);
static_assert(!std::is_move_constructible_v<zaf::Application>);
static_assert(!std::is_move_assignable_v<zaf::Application>);

TEST(ApplicationTest, Initialize) {

    //Application::Initialize() has been called at the entry point of the unit test application.
    //We check whether an exception is thrown if it is called again.
    ASSERT_THROW(zaf::Application::Instance().Initialize({}), zaf::PreconditionError);
}


TEST(ApplicationTest, MainWindowDestroyed) {

    //In this test case, check if there is runtime exception on main window destroyed.

    auto window = zaf::Create<zaf::Window>();
    window->SetInitialRectStyle(zaf::InitialRectStyle::Custom);
    window->SetRect(zaf::Rect{ -100, -100, 100, 100 });
    window->Show();

    zaf::Application::Instance().SetMainWindow(window);

    window->Close();
}