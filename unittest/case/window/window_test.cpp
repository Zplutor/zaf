#include <gtest/gtest.h>
#include <zaf/creation.h>
#include <zaf/graphic/dpi.h>
#include <zaf/window/window.h>

TEST(WindowTest, SetRectBeforeCreate) {

    auto window = zaf::Create<zaf::Window>();
    window->SetInitialRectStyle(zaf::InitialRectStyle::Custom);

    zaf::Rect initial_rect{ 10.25, 20.25, 400.25, 500.25 };
    window->SetRect(initial_rect);
    ASSERT_EQ(window->Rect(), initial_rect);

    window->CreateHandle();
    //After creating the window, Rect() should use the actual window rect.
    //In DIP context, the result may differs from which set by SetRect() due to loss of precision.
    auto pixel_rect = zaf::FromDIPs(initial_rect, window->GetDPI());
    pixel_rect.position.x = std::floor(pixel_rect.position.x);
    pixel_rect.position.y = std::floor(pixel_rect.position.y);
    pixel_rect.size.width = std::floor(pixel_rect.size.width);
    pixel_rect.size.height = std::floor(pixel_rect.size.height);
    auto rect_after_create = zaf::ToDIPs(pixel_rect, window->GetDPI());
    ASSERT_EQ(window->Rect(), rect_after_create);

    window->Destroy();
    //The rect should be reserved after window is destroyed.
    ASSERT_EQ(window->Rect(), rect_after_create);
}


TEST(WindowTest, SetRectAfterCreate) {

    auto window = zaf::Create<zaf::Window>();
    window->CreateHandle();

    zaf::Rect rect{ 10.25, 20.25, 400.25, 500.25 };
    window->SetRect(rect);

    //After calling SetRect(), the rect should be adjusted to the actual window rect.
    auto pixel_rect = zaf::FromDIPs(rect, window->GetDPI());
    pixel_rect.position.x = std::floor(pixel_rect.position.x);
    pixel_rect.position.y = std::floor(pixel_rect.position.y);
    pixel_rect.size.width = std::floor(pixel_rect.size.width);
    pixel_rect.size.height = std::floor(pixel_rect.size.height);
    auto rect_after_set = zaf::ToDIPs(pixel_rect, window->GetDPI());
    ASSERT_EQ(window->Rect(), rect_after_set);

    window->Destroy();
    //The rect should be reserved after window is destroyed.
    ASSERT_EQ(window->Rect(), rect_after_set);
}


TEST(WindowTest, SetContentSize) {

    auto window = zaf::Create<zaf::Window>();
    window->SetRect(zaf::Rect{ 100, 100, 200, 300 });
    window->SetContentSize(zaf::Size{ 200, 300 });

    ASSERT_EQ(window->Position(), zaf::Point(100, 100));

    auto window_size = window->Size();
    ASSERT_GT(window_size.width, 200);
    ASSERT_GT(window_size.height, 300);
    
    ASSERT_EQ(window->ContentSize(), zaf::Size(200, 300));
}