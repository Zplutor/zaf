#include <Windows.h>
#include <gtest/gtest.h>
#include <zaf/base/error/error.h>
#include <zaf/clipboard/clipboard.h>
#include <zaf/creation.h>
#include <zaf/window/window.h>

using namespace zaf::clipboard;

TEST(ClipboardTest, SetText) {

    constexpr const wchar_t* TestText = L"Zaf clipboard unit test";

    Clipboard::SetText(TestText);
    auto text = Clipboard::GetText();
    ASSERT_EQ(text, TestText);

    auto clipboard_window = zaf::Create<zaf::Window>();
    auto window_holder = clipboard_window->CreateHandle();

    OpenClipboard(clipboard_window->Handle());
    ASSERT_THROW(Clipboard::SetText(L"asdfaa"), zaf::Error);
    CloseClipboard();
}