#include <Windows.h>
#include <gtest/gtest.h>
#include <zaf/base/error/invalid_type_error.h>
#include <zaf/base/error/win32_error.h>
#include <zaf/base/global_mem.h>
#include <zaf/clipboard/clipboard.h>
#include <zaf/clipboard/drop_files_data.h>
#include <zaf/clipboard/text_data.h>
#include <zaf/creation.h>
#include <zaf/object/boxing/string.h>
#include <zaf/window/window.h>

using namespace zaf::clipboard;

static_assert(!std::is_default_constructible_v<Clipboard>);

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
        ASSERT_THROW(Clipboard::GetText(), zaf::Win32Error);
        CloseClipboard();
    }

    //Not unicode text format.
    {
        OpenClipboard(nullptr);
        EmptyClipboard();

        auto memory = zaf::GlobalMem::FromString(L"test", zaf::GlobalMemFlags::Movable);
        HANDLE data = SetClipboardData(CF_PRIVATEFIRST + 1, memory.Detach());
        ASSERT_NE(data, nullptr);

        ASSERT_THROW(Clipboard::GetText(), zaf::Win32Error);
        CloseClipboard();
    }
}


TEST(ClipboardTest, SetTextFail) {

    //Fails to open clipboard.
    auto clipboard_window = zaf::Create<zaf::Window>();
    auto window_holder = clipboard_window->CreateHandle();

    OpenClipboard(clipboard_window->Handle());
    ASSERT_THROW(Clipboard::SetText(L"asdfaa"), zaf::Win32Error);
    CloseClipboard();
}


TEST(ClipboardTest, SetDataObject) {

    auto text = L"SetDataObject" + std::to_wstring(std::time(nullptr));

    auto data_object = DataObject::Create();
    data_object.SetText(text);
    Clipboard::SetDataObject(data_object);

    auto text_in_clipboard = Clipboard::GetText();
    ASSERT_EQ(text_in_clipboard, text);

    //Set a null data object to empty the clipboard.
    Clipboard::SetDataObject({});
    ASSERT_THROW(Clipboard::GetText(), zaf::Win32Error);
}