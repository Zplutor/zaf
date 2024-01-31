#include <gtest/gtest.h>
#include <zaf/control/text_box.h>
#include <zaf/creation.h>
#include <zaf/window/window.h>

TEST(TextBoxTest, IsReadOnly) {

    auto window = zaf::Create<zaf::Window>();
    window->SetInitialRectStyle(zaf::InitialRectStyle::Custom);
    window->SetRect(zaf::Rect{ 0, 0, 100, 100 });

    auto text_box = zaf::Create<zaf::TextBox>();
    window->SetRootControl(text_box);

    window->Show();

    //TextBox is read-only by default.
    ASSERT_TRUE(text_box->IsReadOnly());

    //A read-only TextBox won't respond to user input.
    text_box->SetIsFocused(true);
    ASSERT_TRUE(text_box->IsFocused());
    window->Messager().Send(WM_CHAR, L'A', 0);
    ASSERT_EQ(text_box->Text(), std::wstring{});

    //An editable TextBox will respond to user input.
    text_box->SetIsReadOnly(false);
    window->Messager().Send(WM_CHAR, L'A', 0);
    ASSERT_EQ(text_box->Text(), std::wstring{ L'A' });

    window->Destroy();
}


TEST(TextBoxTest, SelectionRange) {

    auto text_box = zaf::Create<zaf::TextBox>();
    ASSERT_EQ(text_box->SelectionRange(), zaf::Range());

    text_box->SetText(L"1234567890");
    text_box->SetSelectionRange(zaf::Range::Infinite());
    ASSERT_EQ(text_box->SelectionRange(), zaf::Range(0, 10));

    text_box->SetSelectionRange(zaf::Range{ 5, 20 });
    ASSERT_EQ(text_box->SelectionRange(), zaf::Range(5, 5));

    text_box->SetSelectionRange(zaf::Range{ 10, 20 });
    ASSERT_EQ(text_box->SelectionRange(), zaf::Range(10, 0));

    text_box->SetSelectionRange(zaf::Range{ 20, 20 });
    ASSERT_EQ(text_box->SelectionRange(), zaf::Range(10, 0));

    //Set caret to the beginning of the range.
    text_box->SetSelectionRange(
        zaf::Range{ 2, 3 }, 
        zaf::text_box::SelectionOption::SetCaretToBegin);
    ASSERT_EQ(text_box->SelectionRange(), zaf::Range(2, 3));
    ASSERT_EQ(text_box->CaretIndex(), 2);

    //Set caret to the end of the range.
    text_box->SetSelectionRange(
        zaf::Range{ 2, 3 },
        zaf::text_box::SelectionOption::SetCaretToEnd);
    ASSERT_EQ(text_box->SelectionRange(), zaf::Range(2, 3));
    ASSERT_EQ(text_box->CaretIndex(), 5);

    //Selection range should be reset after text changed.
    text_box->SetSelectionRange(zaf::Range::Infinite());
    text_box->SetText(L"abcde");
    ASSERT_EQ(text_box->SelectionRange(), zaf::Range());
}


TEST(TextBoxTest, SelectedText) {

    auto text_box = zaf::Create<zaf::TextBox>();
    ASSERT_EQ(text_box->SelectedText(), L"");

    text_box->SetText(L"0123456789");
    text_box->SetSelectionRange(zaf::Range{ 4, 0 });
    ASSERT_EQ(text_box->SelectedText(), L"");

    text_box->SetSelectionRange(zaf::Range{ 3, 1 });
    ASSERT_EQ(text_box->SelectedText(), L"3");

    text_box->SetSelectionRange(zaf::Range{ 10, 1 });
    ASSERT_EQ(text_box->SelectedText(), L"");

    text_box->SetSelectionRange(zaf::Range{ 0, 10 });
    ASSERT_EQ(text_box->SelectedText(), L"0123456789");
}


TEST(TextBoxTest, IsPositionInsideText) {

    auto text_box = zaf::Create<zaf::TextBox>();
    text_box->SetText(L"IsPositionInsideText");

    auto preferred_size = text_box->CalculatePreferredSize();
    text_box->SetSize(preferred_size);

    ASSERT_TRUE(text_box->IsPositionInsideText(zaf::Point{ 0, 0 }));
    ASSERT_FALSE(text_box->IsPositionInsideText(zaf::Point{ -1, 0 }));
    ASSERT_FALSE(text_box->IsPositionInsideText(zaf::Point{ 0, -1 }));
    ASSERT_FALSE(text_box->IsPositionInsideText(zaf::Point{ -1, -1 }));
    ASSERT_FALSE(text_box->IsPositionInsideText(zaf::Point{ preferred_size.width + 1, 0 }));
    ASSERT_FALSE(text_box->IsPositionInsideText(zaf::Point{ 0, preferred_size.height + 1 }));
    ASSERT_FALSE(text_box->IsPositionInsideText(zaf::Point{ 
        preferred_size.width + 1, preferred_size.height + 1
    }));
}