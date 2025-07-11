#include <gtest/gtest.h>
#include <zaf/application.h>
#include <zaf/creation.h>
#include <zaf/window/window.h>
#include "unit_test_delegate.h"

static_assert(!std::is_copy_constructible_v<zaf::Application>);
static_assert(!std::is_copy_assignable_v<zaf::Application>);
static_assert(!std::is_move_constructible_v<zaf::Application>);
static_assert(!std::is_move_assignable_v<zaf::Application>);

TEST(ApplicationTest, Initialize) {

    //Application::Initialize() has been called at the entry point of the unit test application.
    //We check whether an exception is thrown if it is called again.
    ASSERT_THROW(zaf::Application::Instance().Initialize({}), zaf::PreconditionError);
}


TEST(ApplicationTest, ReportUnhandledException_Handled) {

    std::string error_message;

    auto& unit_test_delegate = zaf::testing::UnitTestDelegate::Instance();
    auto guard = unit_test_delegate.RegisterUnhandledExceptionHandler(
        [&](zaf::UnhandledExceptionInfo& event_info) {
            try {
                std::rethrow_exception(event_info.Exception());
            }
            catch (const std::string& error) {
                error_message = error;
            }
            event_info.MarkAsHandled();
        });

    zaf::Application::Instance().ReportUnhandledException(
        std::make_exception_ptr(std::string("HandledError")));

    ASSERT_EQ(error_message, "HandledError");
}


TEST(ApplicationTest, ReportUnhandledException_Unhandled) {

    auto& unit_test_delegate = zaf::testing::UnitTestDelegate::Instance();
    auto guard = unit_test_delegate.RegisterUnhandledExceptionHandler(
        [&](zaf::UnhandledExceptionInfo& event_info) {
            // Do nothing
        });

    std::string error_message;
    try {
        zaf::Application::Instance().ReportUnhandledException(
            std::make_exception_ptr(std::string("UnhandledError")));
    }
    catch (const std::string& error) {
        error_message = error;
    }
    ASSERT_EQ(error_message, "UnhandledError");
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