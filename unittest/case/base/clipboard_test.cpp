#include <Windows.h>
#include <gtest/gtest.h>
#include <zaf/base/clipboard.h>
#include <zaf/base/error/error.h>
#include <zaf/creation.h>
#include <zaf/window/window.h>

TEST(ClipboardTest, SetText) {

    constexpr const wchar_t* TestText = L"Zaf clipboard unit test";

    zaf::Clipboard::SetText(TestText);
    auto text = zaf::Clipboard::GetText();
    ASSERT_EQ(text, TestText);

    auto clipboard_window = zaf::Create<zaf::Window>();
    auto window_holder = clipboard_window->CreateHandle();

    OpenClipboard(clipboard_window->Handle());
    ASSERT_THROW(zaf::Clipboard::SetText(L"asdfaa"), zaf::Error);
    CloseClipboard();
}