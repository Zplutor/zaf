#include <gtest/gtest.h>
#include <zaf/creation.h>
#include <zaf/window/window.h>
#include <zaf/window/window_holder.h>

TEST(WindowHolderTest, CreateMultipleTimesWithoutDestroy) {

    auto window = zaf::Create<zaf::Window>();

    auto holder1 = window->CreateHandle();
    ASSERT_NE(holder1, nullptr);
    ASSERT_EQ(holder1->Window(), window);

    auto holder2 = window->CreateHandle();
    ASSERT_EQ(holder1, holder2);
}


TEST(WindowHolderTest, DestroyHandleOnHolderDestruction) {

    auto window = zaf::Create<zaf::Window>();
    auto holder = window->CreateHandle();

    HWND handle = window->Handle();
    ASSERT_TRUE(IsWindow(handle));

    holder.reset();
    ASSERT_EQ(window->Handle(), nullptr);
    ASSERT_FALSE(IsWindow(handle));
}


TEST(WindowHolderTest, HoldWindow) {

    auto window = zaf::Create<zaf::Window>();
    auto holder = window->CreateHandle();

    HWND handle = window->Handle();
    window.reset();
    ASSERT_TRUE(IsWindow(handle));
    ASSERT_NE(holder->Window(), nullptr);

    window = zaf::GetWindowFromHandle(handle);
    ASSERT_NE(window, nullptr);
    ASSERT_EQ(holder->Window(), window);

    window.reset();
    holder.reset();
    ASSERT_FALSE(IsWindow(handle));
}


TEST(WindowHolderTest, DestroyHandleWhileHolderLives) {

    auto window = zaf::Create<zaf::Window>();
    auto holder = window->CreateHandle();

    window->Destroy();
    ASSERT_EQ(holder->Window(), nullptr);
}


TEST(WindowHolderTest, ShowWindow) {

    auto window = zaf::Create<zaf::Window>();
    window->SetRect(zaf::Rect{ 0, 0, 1, 1 });
    window->Show();

    HWND handle = window->Handle();
    window.reset();
    ASSERT_TRUE(IsWindow(handle));

    window = zaf::GetWindowFromHandle(handle);
    ASSERT_NE(window, nullptr);

    window->Destroy();
    ASSERT_FALSE(IsWindow(handle));
}


TEST(WindowHolderTest, GetHolderAndShowWindow) {

    auto window = zaf::Create<zaf::Window>();
    window->SetRect(zaf::Rect{ 0, 0, 1, 1 });

    auto holder = window->CreateHandle();
    window->Show();

    HWND handle = window->Handle();
    window.reset();
    holder.reset();
    ASSERT_TRUE(IsWindow(handle));

    window = zaf::GetWindowFromHandle(handle);
    ASSERT_NE(window, nullptr);

    window->Destroy();
    ASSERT_FALSE(IsWindow(handle));
}
