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

    zaf::Point window_position{ 10.25, 10.25 };
    zaf::Size content_size{ 200.25, 300.25 };
    window->SetRect(zaf::Rect{ window_position, content_size });
    window->SetContentSize(content_size);

    //Position should not changed after setting content size.
    ASSERT_EQ(window->Position(), window_position);

    //Window size should be enlarged.
    auto window_size = window->Size();
    ASSERT_GT(window_size.width, content_size.width);
    ASSERT_GT(window_size.height, content_size.height);
    
    //Even if window handle is not created yet, content size would be adjusted.
    zaf::Size adjusted_content_size;
    adjusted_content_size.width = std::floor(content_size.width);
    adjusted_content_size.height = std::floor(content_size.height);
    ASSERT_EQ(window->ContentSize(), adjusted_content_size);

    window->CreateHandle();

    //After creating handle, content size should not changed.
    ASSERT_EQ(window->ContentSize(), adjusted_content_size);
}