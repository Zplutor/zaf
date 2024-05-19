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


TEST(ClipboardTest, RegisteredClipboardData) {

    //Register private format type.
    Clipboard::RegisterClipboardData(MakePrivateFormatType(1), TextData::Type);
    auto object_type = Clipboard::GetRegisteredClipboardData(MakePrivateFormatType(1));
    ASSERT_EQ(object_type, TextData::Type);

    //Register type which doesn't inherit from ClipboardData.
    ASSERT_THROW(
        Clipboard::RegisterClipboardData(MakePrivateFormatType(2), zaf::WideString::Type),
        zaf::InvalidTypeError);

    ASSERT_THROW(
        Clipboard::RegisterClipboardData(MakePrivateFormatType(3), ClipboardData::Type),
        zaf::InvalidTypeError);

    //Register null.
    ASSERT_THROW(
        Clipboard::RegisterClipboardData(MakePrivateFormatType(4), nullptr),
        zaf::PreconditionError);
}


TEST(ClipboardTest, GetRegisteredClipboardData) {

    //Get type of standard format type.
    auto object_type = Clipboard::GetRegisteredClipboardData(FormatType::Text);
    ASSERT_EQ(object_type, TextData::Type);
    ASSERT_NE(object_type, ClipboardData::Type);

    object_type = Clipboard::GetRegisteredClipboardData(FormatType::DropFiles);
    ASSERT_EQ(object_type, DropFilesData::Type);
    ASSERT_NE(object_type, ClipboardData::Type);

    //Get unregistered format type.
    object_type = Clipboard::GetRegisteredClipboardData(FormatType::Indeterminate);
    ASSERT_EQ(object_type, nullptr);
}