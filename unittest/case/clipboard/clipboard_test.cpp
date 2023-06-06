#include <Windows.h>
#include <gtest/gtest.h>
#include <zaf/base/error/error.h>
#include <zaf/base/global_mem.h>
#include <zaf/clipboard/clipboard.h>
#include <zaf/creation.h>
#include <zaf/window/window.h>

using namespace zaf::clipboard;

TEST(ClipboardTest, GetAndSetText) {

    constexpr const wchar_t* TestText = L"Zaf clipboard unit test";

    Clipboard::SetText(TestText);
    auto text = Clipboard::GetText();
    ASSERT_EQ(text, TestText);
}


TEST(ClipboardTest, GetTextFail) {

    //Fails to open clipboard.
    {
        auto clipboard_window = zaf::Create<zaf::Window>();
        auto window_holder = clipboard_window->CreateHandle();

        OpenClipboard(clipboard_window->Handle());
        ASSERT_THROW(Clipboard::GetText(), zaf::Error);
        CloseClipboard();
    }

    //Not unicode text format.
    {
        OpenClipboard(nullptr);
        EmptyClipboard();

        auto memory = zaf::GlobalMem::FromString(L"test", zaf::GlobalMemFlags::Movable);
        HANDLE data = SetClipboardData(CF_PRIVATEFIRST + 1, memory.Detach());
        ASSERT_NE(data, nullptr);

        ASSERT_THROW(Clipboard::GetText(), zaf::Error);
        CloseClipboard();
    }
}


TEST(ClipboardTest, SetTextFail) {

    //Fails to open clipboard.
    auto clipboard_window = zaf::Create<zaf::Window>();
    auto window_holder = clipboard_window->CreateHandle();

    OpenClipboard(clipboard_window->Handle());
    ASSERT_THROW(Clipboard::SetText(L"asdfaa"), zaf::Error);
    CloseClipboard();
}