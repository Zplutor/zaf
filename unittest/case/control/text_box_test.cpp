#include <gtest/gtest.h>
#include <zaf/control/text_box.h>
#include <zaf/creation.h>
#include <zaf/window/window.h>
#include "utility/test_window.h"

namespace {

void TestWithTextBoxInWindow(
    const std::function<void(zaf::TextBox& text_box, zaf::Window& window)>& test) {

    TestWithWindow([test](zaf::Window& window) {
    
        auto text_box = zaf::Create<zaf::TextBox>();
        window.SetRootControl(text_box);
        text_box->SetIsFocused(true);
        text_box->SetIsEditable(true);

        test(*text_box, window);
    });
}

}

TEST(TextBoxTest, IsEditable) {

    //TextBox is not editable by default.
    ASSERT_FALSE(zaf::Create<zaf::TextBox>()->IsEditable());

    TestWithTextBoxInWindow([](zaf::TextBox& text_box, zaf::Window& window) {

        text_box.SetIsEditable(false);

        //A non-editable TextBox won't respond to user input.
        window.Messager().Send(WM_CHAR, L'A', 0);
        ASSERT_EQ(text_box.Text(), std::wstring{});

        //An editable TextBox will respond to user input.
        text_box.SetIsEditable(true);
        window.Messager().Send(WM_CHAR, L'A', 0);
        ASSERT_EQ(text_box.Text(), std::wstring{ L'A' });
    });
}


TEST(TextBoxTest, AllowUndo) {

    //TextBox allows undo by default.
    ASSERT_TRUE(zaf::Create<zaf::TextBox>()->AllowUndo());

    TestWithTextBoxInWindow([](zaf::TextBox& text_box, zaf::Window& window) {

        //TextBox cannot undo after disable undo.
        window.Messager().Send(WM_CHAR, L'B', 0);
        text_box.SetAllowUndo(false);
        ASSERT_FALSE(text_box.CanUndo());
        ASSERT_FALSE(text_box.Undo());

        window.Messager().Send(WM_CHAR, L'B', 0);
        ASSERT_FALSE(text_box.CanUndo());
        ASSERT_FALSE(text_box.Undo());
    });
}


TEST(TextBoxTest, CanUndoCanRedo) {

    TestWithTextBoxInWindow([](zaf::TextBox& text_box, zaf::Window& window) {

        //TextBox cannot undo nor redo at first.
        ASSERT_FALSE(text_box.CanUndo());
        ASSERT_FALSE(text_box.CanRedo());

        //Can undo after input.
        window.Messager().Send(WM_CHAR, L'C', 0);
        ASSERT_TRUE(text_box.CanUndo());
        ASSERT_FALSE(text_box.CanRedo());

        //Can redo after undo.
        text_box.Undo();
        ASSERT_FALSE(text_box.CanUndo());
        ASSERT_TRUE(text_box.CanRedo());

        //Can undo after redo.
        text_box.Redo();
        ASSERT_TRUE(text_box.CanUndo());
        ASSERT_FALSE(text_box.CanRedo());

        //Can both undo and redo.
        window.Messager().Send(WM_CHAR, L'D', 0);
        text_box.Undo();
        ASSERT_TRUE(text_box.CanUndo());
        ASSERT_TRUE(text_box.CanRedo());

        //Cannot undo nor redo after SetText();
        text_box.SetText(L"clear");
        ASSERT_FALSE(text_box.CanUndo());
        ASSERT_FALSE(text_box.CanRedo());
    });
}


TEST(TextBoxTest, UndoRedo) {

    TestWithTextBoxInWindow([](zaf::TextBox& text_box, zaf::Window& window) {

        //Cannot undo nor redo by default.
        ASSERT_FALSE(text_box.Undo());
        ASSERT_FALSE(text_box.Redo());

        //Undo
        window.Messager().Send(WM_CHAR, L'1', 0);
        ASSERT_TRUE(text_box.Undo());
        ASSERT_EQ(text_box.Text(), std::wstring{});

        //Redo
        ASSERT_TRUE(text_box.Redo());
        ASSERT_EQ(text_box.Text(), L"1");

        window.Messager().Send(WM_CHAR, L'2', 0);
        window.Messager().Send(WM_CHAR, L'3', 0);
        window.Messager().Send(WM_CHAR, L'4', 0);
        ASSERT_TRUE(text_box.Undo());
        ASSERT_EQ(text_box.Text(), L"123");
        ASSERT_TRUE(text_box.Undo());
        ASSERT_EQ(text_box.Text(), L"12");
        ASSERT_TRUE(text_box.Undo());
        ASSERT_EQ(text_box.Text(), L"1");

        ASSERT_TRUE(text_box.Redo());
        ASSERT_EQ(text_box.Text(), L"12");
        ASSERT_TRUE(text_box.Redo());
        ASSERT_EQ(text_box.Text(), L"123");
        ASSERT_TRUE(text_box.Redo());
        ASSERT_EQ(text_box.Text(), L"1234");
    });
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


/*
TEST(TextBoxTest, HandleCtrlBackspace) {

    TestWithTextBoxInWindow([](zaf::TextBox& text_box, zaf::Window& window) {
    
        text_box.SetText(L" abc123 ");
        text_box.SetSelectionRange(zaf::Range{ 4, 0 });

        window.Messager().Send(WM_KEYDOWN, static_cast<WPARAM>(zaf::Key::Ctrl), 0);
        window.Messager().Send(WM_KEYDOWN, static_cast<WPARAM>(zaf::Key::Backspace), 0);
        ASSERT_EQ(text_box.Text(), L" 123 ");
        ASSERT_EQ(text_box.SelectionRange(), zaf::Range(1, 0));
    });
}
*/