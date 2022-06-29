#include <gtest/gtest.h>
#include <zaf/creation.h>
#include <zaf/graphic/alignment.h>
#include <zaf/graphic/dpi.h>
#include <zaf/window/window.h>

TEST(WindowTest, ClassName) {

    //Default class name
    auto window = zaf::Create<zaf::Window>();
    ASSERT_EQ(window->ClassName(), zaf::Window::DefaultClassName);

    window->CreateHandle();
    wchar_t buffer[100]{};
    GetClassName(window->Handle(), buffer, 100);
    ASSERT_EQ(std::wcscmp(buffer, zaf::Window::DefaultClassName), 0);

    //Custom class name
    const std::wstring custom_class_name(L"CustomClassName");
    window = zaf::Create<zaf::Window>(custom_class_name);
    ASSERT_EQ(window->ClassName(), custom_class_name);

    window->CreateHandle();
    GetClassName(window->Handle(), buffer, 100);
    ASSERT_EQ(std::wcscmp(buffer, custom_class_name.c_str()), 0);
}


TEST(WindowTest, SetRectBeforeCreate) {

    auto window = zaf::Create<zaf::Window>();
    window->SetInitialRectStyle(zaf::InitialRectStyle::Custom);

    zaf::Rect initial_rect{ 10.25, 20.25, 400.25, 500.25 };
    window->SetRect(initial_rect);
    ASSERT_EQ(window->Rect(), initial_rect);

    window->CreateHandle();
    //After creating the window, Rect() should use the actual window rect.
    auto pixel_rect = zaf::Align(zaf::FromDIPs(initial_rect, window->GetDPI()));
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
    auto pixel_rect = zaf::Align(zaf::FromDIPs(rect, window->GetDPI()));
    auto rect_after_set = zaf::ToDIPs(pixel_rect, window->GetDPI());
    ASSERT_EQ(window->Rect(), rect_after_set);

    window->Destroy();
    //The rect should be reserved after window is destroyed.
    ASSERT_EQ(window->Rect(), rect_after_set);
}


TEST(WindowTest, GetRectAfterChangeRect) {

    auto window = zaf::Create<zaf::Window>();
    window->SetInitialRectStyle(zaf::InitialRectStyle::Custom);
    window->SetRect(zaf::Rect{ 100, 100, 400, 400 });

    //Simulate user resize the window.
    window->CreateHandle();
    SetWindowPos(
        window->Handle(),
        nullptr,
        0,
        0,
        static_cast<int>(zaf::FromDIPs(400, window->GetDPI())),
        static_cast<int>(zaf::FromDIPs(500, window->GetDPI())),
        SWP_NOMOVE | SWP_NOACTIVATE);
    window->Destroy();

    zaf::Rect new_rect{ 100, 100, 400, 500 };
    ASSERT_EQ(window->Rect(), new_rect);

    //Simulate user move the window.
    window->CreateHandle();
    SetWindowPos(
        window->Handle(),
        nullptr,
        static_cast<int>(zaf::FromDIPs(200, window->GetDPI())),
        static_cast<int>(zaf::FromDIPs(300, window->GetDPI())),
        0,
        0,
        SWP_NOSIZE | SWP_NOACTIVATE);
    window->Destroy();

    new_rect = zaf::Rect{ 200, 300, 400, 500 };
    ASSERT_EQ(window->Rect(), new_rect);
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