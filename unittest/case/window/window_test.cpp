#include <gtest/gtest.h>
#include <zaf/creation.h>
#include <zaf/window/window.h>

TEST(WindowTest, SetContentSize) {

    auto window = zaf::Create<zaf::Window>();
    window->SetRect(zaf::Rect{ 100, 100, 200, 300 });
    window->SetContentSize(zaf::Size{ 200, 300 });

    ASSERT_EQ(window->Position(), zaf::Point(100, 100));

    auto window_size = window->Size();
    ASSERT_GT(window_size.width, 200);
    ASSERT_GT(window_size.height, 300);
}