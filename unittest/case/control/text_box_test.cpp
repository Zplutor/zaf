#include <gtest/gtest.h>
#include <zaf/clipboard/clipboard.h>
#include <zaf/control/text_box.h>
#include <zaf/control/textual/inline_object.h>
#include <zaf/control/textual/styled_text.h>
#include <zaf/creation.h>
#include <zaf/window/window.h>
#include "utility/test_window.h"

using namespace zaf;
using namespace zaf::textual;

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


TEST(TextBoxTest, MaxLength) {

    auto control = Create<TextBox>();
    ASSERT_FALSE(control->MaxLength().has_value());
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
        zaf::textual::SelectionOption::SetCaretToBegin);
    ASSERT_EQ(text_box->SelectionRange(), zaf::Range(2, 3));
    ASSERT_EQ(text_box->CaretIndex(), 2);

    //Set caret to the end of the range.
    text_box->SetSelectionRange(
        zaf::Range{ 2, 3 },
        zaf::textual::SelectionOption::SetCaretToEnd);
    ASSERT_EQ(text_box->SelectionRange(), zaf::Range(2, 3));
    ASSERT_EQ(text_box->CaretIndex(), 5);

    //Selection range should be reset after text changed.
    text_box->SetSelectionRange(zaf::Range::Infinite());
    text_box->SetText(L"abcde");
    ASSERT_EQ(text_box->SelectionRange(), zaf::Range());
}


TEST(TextBoxTest, SelectionChangedEvent) {

    auto control = Create<TextBox>();
    control->SetText(L"TextBox");

    int event_call_count{};
    auto sub = control->SelectionChangedEvent().Subscribe(
        [&event_call_count](const SelectionChangedInfo& event_info) {
        ++event_call_count;
    });

    control->SetSelectionRange(Range{ 1, 0 });
    control->SetSelectionRange(Range{ 1, 1 });
    control->SetSelectionRange(Range{ 0, 7 });
    ASSERT_EQ(event_call_count, 3);
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


TEST(TextBoxTest, BackwardCaretByLeftKey) {

    TestWithTextBoxInWindow([](TextBox& text_box, Window& window) {

        //Backward in normal text
        text_box.SetText(L"abc");
        text_box.SetSelectionRange(Range{ 3, 0 });
        window.Messager().SendWMKEYDOWN(zaf::Key::Left);
        ASSERT_EQ(text_box.SelectionRange(), Range(2, 0)); // c
        window.Messager().SendWMKEYDOWN(zaf::Key::Left);
        ASSERT_EQ(text_box.SelectionRange(), Range(1, 0)); // b
        window.Messager().SendWMKEYDOWN(zaf::Key::Left);
        ASSERT_EQ(text_box.SelectionRange(), Range(0, 0)); // a
        window.Messager().SendWMKEYDOWN(zaf::Key::Left);
        ASSERT_EQ(text_box.SelectionRange(), Range(0, 0)); // a

        //Backward in empty lines
        text_box.SetText(L"\r\rH\n\n\r\n\r\n");
        text_box.SetSelectionRange(Range{ 9, 0 });
        window.Messager().SendWMKEYDOWN(zaf::Key::Left);
        ASSERT_EQ(text_box.SelectionRange(), Range(7, 0)); // \r\n
        window.Messager().SendWMKEYDOWN(zaf::Key::Left);
        ASSERT_EQ(text_box.SelectionRange(), Range(5, 0)); // \r\n
        window.Messager().SendWMKEYDOWN(zaf::Key::Left);
        ASSERT_EQ(text_box.SelectionRange(), Range(4, 0)); // \n
        window.Messager().SendWMKEYDOWN(zaf::Key::Left);
        ASSERT_EQ(text_box.SelectionRange(), Range(3, 0)); // \n
        window.Messager().SendWMKEYDOWN(zaf::Key::Left);
        ASSERT_EQ(text_box.SelectionRange(), Range(2, 0)); // H
        window.Messager().SendWMKEYDOWN(zaf::Key::Left);
        ASSERT_EQ(text_box.SelectionRange(), Range(1, 0)); // \r
        window.Messager().SendWMKEYDOWN(zaf::Key::Left);
        ASSERT_EQ(text_box.SelectionRange(), Range(0, 0)); // \r
        window.Messager().SendWMKEYDOWN(zaf::Key::Left);
        ASSERT_EQ(text_box.SelectionRange(), Range(0, 0)); // \r

        //Backward to the beginning of the selection.
        text_box.SetText(L"selection");
        text_box.SetSelectionRange(Range{ 3, 3 });
        window.Messager().SendWMKEYDOWN(Key::Left);
        ASSERT_EQ(text_box.SelectionRange(), Range(3, 0));

        //Backward across inline objects.
        text_box.SetText(L"xxOxOOxx");
        text_box.AttachInlineObjectToRange(Create<InlineObject>(), Range{ 2, 1 });
        text_box.AttachInlineObjectToRange(Create<InlineObject>(), Range{ 4, 2 });
        text_box.SetSelectionRange(Range{ 6, 0 });
        window.Messager().SendWMKEYDOWN(Key::Left);
        ASSERT_EQ(text_box.SelectionRange(), Range(4, 0));
        window.Messager().SendWMKEYDOWN(Key::Left);
        ASSERT_EQ(text_box.SelectionRange(), Range(3, 0));
        window.Messager().SendWMKEYDOWN(Key::Left);
        ASSERT_EQ(text_box.SelectionRange(), Range(2, 0));
    });
}


TEST(TextBoxTest, ForwardCaretByRightKey) {

    TestWithTextBoxInWindow([](TextBox& text_box, Window& window) {

        //Forward in normal text
        text_box.SetText(L"abc");
        text_box.SetSelectionRange(Range{ 0, 0 });
        window.Messager().SendWMKEYDOWN(zaf::Key::Right);
        ASSERT_EQ(text_box.SelectionRange(), Range(1, 0)); // b
        window.Messager().SendWMKEYDOWN(zaf::Key::Right);
        ASSERT_EQ(text_box.SelectionRange(), Range(2, 0)); // c
        window.Messager().SendWMKEYDOWN(zaf::Key::Right);
        ASSERT_EQ(text_box.SelectionRange(), Range(3, 0)); 
        window.Messager().SendWMKEYDOWN(zaf::Key::Right);
        ASSERT_EQ(text_box.SelectionRange(), Range(3, 0)); 

        //Forward in empty lines
        text_box.SetText(L"\r\rH\n\n\r\n\r\n");
        text_box.SetSelectionRange(Range{ 0, 0 });
        window.Messager().SendWMKEYDOWN(zaf::Key::Right);
        ASSERT_EQ(text_box.SelectionRange(), Range(1, 0)); // \r
        window.Messager().SendWMKEYDOWN(zaf::Key::Right);
        ASSERT_EQ(text_box.SelectionRange(), Range(2, 0)); // H
        window.Messager().SendWMKEYDOWN(zaf::Key::Right);
        ASSERT_EQ(text_box.SelectionRange(), Range(3, 0)); // \n
        window.Messager().SendWMKEYDOWN(zaf::Key::Right);
        ASSERT_EQ(text_box.SelectionRange(), Range(4, 0)); // \n
        window.Messager().SendWMKEYDOWN(zaf::Key::Right);
        ASSERT_EQ(text_box.SelectionRange(), Range(5, 0)); // \r\n
        window.Messager().SendWMKEYDOWN(zaf::Key::Right);
        ASSERT_EQ(text_box.SelectionRange(), Range(7, 0)); // \r\n
        window.Messager().SendWMKEYDOWN(zaf::Key::Right);
        ASSERT_EQ(text_box.SelectionRange(), Range(9, 0));
        window.Messager().SendWMKEYDOWN(zaf::Key::Right);
        ASSERT_EQ(text_box.SelectionRange(), Range(9, 0));

        //Forward to the end of the selection.
        text_box.SetText(L"selection");
        text_box.SetSelectionRange(Range{ 3, 3 });
        window.Messager().SendWMKEYDOWN(Key::Right);
        ASSERT_EQ(text_box.SelectionRange(), Range(6, 0));

        //Forward across inline objects.
        text_box.SetText(L"xxOxOOxx");
        text_box.AttachInlineObjectToRange(Create<InlineObject>(), Range{ 2, 1 });
        text_box.AttachInlineObjectToRange(Create<InlineObject>(), Range{ 4, 2 });
        text_box.SetSelectionRange(Range{ 2, 0 });
        window.Messager().SendWMKEYDOWN(Key::Right);
        ASSERT_EQ(text_box.SelectionRange(), Range(3, 0));
        window.Messager().SendWMKEYDOWN(Key::Right);
        ASSERT_EQ(text_box.SelectionRange(), Range(4, 0));
        window.Messager().SendWMKEYDOWN(Key::Right);
        ASSERT_EQ(text_box.SelectionRange(), Range(6, 0));
    });
}


TEST(TextBoxTest, HandleBackspace) {

    TestWithTextBoxInWindow([](zaf::TextBox& text_box, zaf::Window& window) {
    
        //Backspace on multi-chars selection.
        text_box.SetText(L"selection");
        text_box.SetSelectionRange(Range{ 3, 3 });
        window.Messager().SendWMKEYDOWN(Key::Backspace);
        ASSERT_EQ(text_box.Text(), L"selion");
        ASSERT_EQ(text_box.SelectionRange(), Range(3, 0));
        window.Messager().SendWMKEYDOWN(Key::Backspace);
        ASSERT_EQ(text_box.Text(), L"seion");
        ASSERT_EQ(text_box.SelectionRange(), Range(2, 0));

        //Backspace between lines.
        text_box.SetText(L"a\rb\nc\r\nd");
        text_box.SetSelectionRange(zaf::Range{ 8, 0 });
        window.Messager().SendWMKEYDOWN(Key::Backspace);
        ASSERT_EQ(text_box.Text(), L"a\rb\nc\r\n");
        ASSERT_EQ(text_box.SelectionRange(), zaf::Range(7, 0));
        window.Messager().SendWMKEYDOWN(Key::Backspace);
        ASSERT_EQ(text_box.Text(), L"a\rb\nc");
        ASSERT_EQ(text_box.SelectionRange(), zaf::Range(5, 0));
        window.Messager().SendWMKEYDOWN(Key::Backspace);
        ASSERT_EQ(text_box.Text(), L"a\rb\n");
        ASSERT_EQ(text_box.SelectionRange(), zaf::Range(4, 0));
        window.Messager().SendWMKEYDOWN(Key::Backspace);
        ASSERT_EQ(text_box.Text(), L"a\rb");
        ASSERT_EQ(text_box.SelectionRange(), zaf::Range(3, 0));
        window.Messager().SendWMKEYDOWN(Key::Backspace);
        ASSERT_EQ(text_box.Text(), L"a\r");
        ASSERT_EQ(text_box.SelectionRange(), zaf::Range(2, 0));
        window.Messager().SendWMKEYDOWN(Key::Backspace);
        ASSERT_EQ(text_box.Text(), L"a");
        ASSERT_EQ(text_box.SelectionRange(), zaf::Range(1, 0));
        window.Messager().SendWMKEYDOWN(Key::Backspace);
        ASSERT_EQ(text_box.Text(), L"");
        ASSERT_EQ(text_box.SelectionRange(), zaf::Range(0, 0));
        window.Messager().SendWMKEYDOWN(Key::Backspace);
        ASSERT_EQ(text_box.Text(), L"");
        ASSERT_EQ(text_box.SelectionRange(), zaf::Range(0, 0));

        //Backspace between inline objects.
        text_box.SetText(L"xOxOOx");
        text_box.AttachInlineObjectToRange(Create<InlineObject>(), Range{ 1, 1 });
        text_box.AttachInlineObjectToRange(Create<InlineObject>(), Range{ 3, 2 });
        text_box.SetSelectionRange(Range{ 6, 0 });
        window.Messager().SendWMKEYDOWN(Key::Backspace);
        ASSERT_EQ(text_box.Text(), L"xOxOO");
        ASSERT_EQ(text_box.SelectionRange(), zaf::Range(5, 0));
        window.Messager().SendWMKEYDOWN(Key::Backspace);
        ASSERT_EQ(text_box.Text(), L"xOx");
        ASSERT_EQ(text_box.SelectionRange(), zaf::Range(3, 0));
        window.Messager().SendWMKEYDOWN(Key::Backspace);
        ASSERT_EQ(text_box.Text(), L"xO");
        ASSERT_EQ(text_box.SelectionRange(), zaf::Range(2, 0));
        window.Messager().SendWMKEYDOWN(Key::Backspace);
        ASSERT_EQ(text_box.Text(), L"x");
        ASSERT_EQ(text_box.SelectionRange(), zaf::Range(1, 0));
        window.Messager().SendWMKEYDOWN(Key::Backspace);
        ASSERT_EQ(text_box.Text(), L"");
        ASSERT_EQ(text_box.SelectionRange(), zaf::Range(0, 0));
    });
}


TEST(TextBoxTest, HandleDelete) {

    TestWithTextBoxInWindow([](zaf::TextBox& text_box, zaf::Window& window) {

        //Delete on multi-chars selection.
        text_box.SetText(L"selection");
        text_box.SetSelectionRange(Range{ 3, 3 });
        window.Messager().SendWMKEYDOWN(Key::Delete);
        ASSERT_EQ(text_box.Text(), L"selion");
        ASSERT_EQ(text_box.SelectionRange(), Range(3, 0));
        window.Messager().SendWMKEYDOWN(Key::Delete);
        ASSERT_EQ(text_box.Text(), L"selon");
        ASSERT_EQ(text_box.SelectionRange(), Range(3, 0));

        //Delete between lines.
        text_box.SetText(L"a\rb\nc\r\nd");
        text_box.SetSelectionRange(zaf::Range{ 0, 0 });
        window.Messager().SendWMKEYDOWN(Key::Delete);
        ASSERT_EQ(text_box.Text(), L"\rb\nc\r\nd");
        ASSERT_EQ(text_box.SelectionRange(), zaf::Range(0, 0));
        window.Messager().SendWMKEYDOWN(Key::Delete);
        ASSERT_EQ(text_box.Text(), L"b\nc\r\nd");
        ASSERT_EQ(text_box.SelectionRange(), zaf::Range(0, 0));
        window.Messager().SendWMKEYDOWN(Key::Delete);
        ASSERT_EQ(text_box.Text(), L"\nc\r\nd");
        ASSERT_EQ(text_box.SelectionRange(), zaf::Range(0, 0));
        window.Messager().SendWMKEYDOWN(Key::Delete);
        ASSERT_EQ(text_box.Text(), L"c\r\nd");
        ASSERT_EQ(text_box.SelectionRange(), zaf::Range(0, 0));
        window.Messager().SendWMKEYDOWN(Key::Delete);
        ASSERT_EQ(text_box.Text(), L"\r\nd");
        ASSERT_EQ(text_box.SelectionRange(), zaf::Range(0, 0));
        window.Messager().SendWMKEYDOWN(Key::Delete);
        ASSERT_EQ(text_box.Text(), L"d");
        ASSERT_EQ(text_box.SelectionRange(), zaf::Range(0, 0));
        window.Messager().SendWMKEYDOWN(Key::Delete);
        ASSERT_EQ(text_box.Text(), L"");
        ASSERT_EQ(text_box.SelectionRange(), zaf::Range(0, 0));
        window.Messager().SendWMKEYDOWN(Key::Delete);
        ASSERT_EQ(text_box.Text(), L"");
        ASSERT_EQ(text_box.SelectionRange(), zaf::Range(0, 0));

        //Delete between inline objects.
        text_box.SetText(L"xOxOOx");
        text_box.AttachInlineObjectToRange(Create<InlineObject>(), Range{ 1, 1 });
        text_box.AttachInlineObjectToRange(Create<InlineObject>(), Range{ 3, 2 });
        text_box.SetSelectionRange(Range{ 0, 0 });
        window.Messager().SendWMKEYDOWN(Key::Delete);
        ASSERT_EQ(text_box.Text(), L"OxOOx");
        ASSERT_EQ(text_box.SelectionRange(), zaf::Range(0, 0));
        window.Messager().SendWMKEYDOWN(Key::Delete);
        ASSERT_EQ(text_box.Text(), L"xOOx");
        ASSERT_EQ(text_box.SelectionRange(), zaf::Range(0, 0));
        window.Messager().SendWMKEYDOWN(Key::Delete);
        ASSERT_EQ(text_box.Text(), L"OOx");
        ASSERT_EQ(text_box.SelectionRange(), zaf::Range(0, 0));
        window.Messager().SendWMKEYDOWN(Key::Delete);
        ASSERT_EQ(text_box.Text(), L"x");
        ASSERT_EQ(text_box.SelectionRange(), zaf::Range(0, 0));
        window.Messager().SendWMKEYDOWN(Key::Delete);
        ASSERT_EQ(text_box.Text(), L"");
        ASSERT_EQ(text_box.SelectionRange(), zaf::Range(0, 0));
    });
}


TEST(TextBoxTest, HandleEnd) {

    TestWithTextBoxInWindow([](zaf::TextBox& text_box, zaf::Window& window) {

        //Press END in empty text.
        text_box.SetText(L"");
        text_box.SetSelectionRange({});
        window.Messager().SendWMKEYDOWN(zaf::Key::End);
        ASSERT_EQ(text_box.SelectionRange(), zaf::Range(0, 0));

        //Press END in single line text.
        //Caret is at the beginning.
        text_box.SetText(L"single line");
        text_box.SetSelectionRange({});
        window.Messager().SendWMKEYDOWN(zaf::Key::End);
        ASSERT_EQ(text_box.SelectionRange(), zaf::Range(11, 0));

        //Caret is at the middle.
        text_box.SetText(L"single line");
        text_box.SetSelectionRange(zaf::Range{ 6, 0 });
        window.Messager().SendWMKEYDOWN(zaf::Key::End);
        ASSERT_EQ(text_box.SelectionRange(), zaf::Range(11, 0));

        //Caret is at the end.
        text_box.SetSelectionRange(zaf::Range{ 11, 0 });
        window.Messager().SendWMKEYDOWN(zaf::Key::End);
        ASSERT_EQ(text_box.SelectionRange(), zaf::Range(11, 0));

        //Press END in multi line text.
        text_box.SetText(L"line1\rline2");
        text_box.SetSelectionRange({});
        window.Messager().SendWMKEYDOWN(zaf::Key::End);
        ASSERT_EQ(text_box.SelectionRange(), zaf::Range(5, 0));

        text_box.SetText(L"line1\nline2");
        text_box.SetSelectionRange({});
        window.Messager().SendWMKEYDOWN(zaf::Key::End);
        ASSERT_EQ(text_box.SelectionRange(), zaf::Range(5, 0));

        text_box.SetText(L"line1\r\nline2");
        text_box.SetSelectionRange({});
        window.Messager().SendWMKEYDOWN(zaf::Key::End);
        ASSERT_EQ(text_box.SelectionRange(), zaf::Range(5, 0));
    });
}


TEST(TextBoxTest, HandleEnter) {

    TestWithTextBoxInWindow([](zaf::TextBox& text_box, zaf::Window& window) {

        //Press Enter in empty text.
        text_box.SetText(L"");
        text_box.SetSelectionRange({});
        window.Messager().SendWMKEYDOWN(zaf::Key::Enter);
        ASSERT_EQ(text_box.Text(), L"\r\n");
        ASSERT_EQ(text_box.SelectionRange(), zaf::Range(2, 0));

        //Press Enter at the middle of the text.
        text_box.SetText(L"00000");
        text_box.SetSelectionRange({ 2, 0 });
        window.Messager().SendWMKEYDOWN(zaf::Key::Enter);
        ASSERT_EQ(text_box.Text(), L"00\r\n000");
        ASSERT_EQ(text_box.SelectionRange(), zaf::Range(4, 0));

        //Press Enter with CRLF line break.
        text_box.SetText(L"");
        text_box.SetLineBreak(LineBreak::CRLF);
        window.Messager().SendWMKEYDOWN(zaf::Key::Enter);
        ASSERT_EQ(text_box.Text(), L"\r\n");

        //Press Enter with CR line break.
        text_box.SetText(L"");
        text_box.SetLineBreak(LineBreak::CR);
        window.Messager().SendWMKEYDOWN(zaf::Key::Enter);
        ASSERT_EQ(text_box.Text(), L"\r");

        //Press Enter with LF line break.
        text_box.SetText(L"");
        text_box.SetLineBreak(LineBreak::LF);
        window.Messager().SendWMKEYDOWN(zaf::Key::Enter);
        ASSERT_EQ(text_box.Text(), L"\n");

        //Press Enter in a single line control.
        text_box.SetIsMultiline(false);
        text_box.SetText(L"11222");
        text_box.SetSelectionRange({ 2, 0 });
        window.Messager().SendWMKEYDOWN(zaf::Key::Enter);
        //Won't insert a line break.
        ASSERT_EQ(text_box.Text(), L"11222");
        //Won't change the undo history.
        ASSERT_FALSE(text_box.CanUndo());
    });
}


TEST(TextBoxTest, Copy) {

    auto control = zaf::Create<zaf::TextBox>();
    control->SetText(L"Text to copy");

    control->SetSelectionRange(zaf::Range{ 2, 0 });
    //Returns false if nothing is selected.
    ASSERT_FALSE(control->Copy());

    control->SetSelectionRange(zaf::Range{ 5, 7 });
    ASSERT_TRUE(control->Copy());
    ASSERT_EQ(zaf::clipboard::Clipboard::GetText(), L"to copy");
}


TEST(TextBoxTest, CopingEvent) {

    auto control = Create<TextBox>();
    control->SetText(L"Text");
    control->SetSelectionRange({ 0, 4 });

    auto sub = control->CopyingEvent().Subscribe([](const CopyingInfo& event_info) {
        event_info.MarkAsHandled();
    });

    clipboard::Clipboard::SetText(L"");
    //No data object is provided, copy will fail.
    ASSERT_FALSE(control->Copy());
    ASSERT_EQ(clipboard::Clipboard::GetText(), L"");

    sub = control->CopyingEvent().Subscribe([](const CopyingInfo& event_info) {
        auto data_object = clipboard::DataObject::Create();
        data_object.SetText(L"text from copying event");
        event_info.SetDataObject(std::move(data_object));
        event_info.MarkAsHandled();
    });
    ASSERT_TRUE(control->Copy());
    ASSERT_EQ(clipboard::Clipboard::GetText(), L"text from copying event");
}


TEST(TextBoxTest, Cut) {

    auto control = zaf::Create<zaf::TextBox>();
    control->SetIsEditable(false);
    control->SetText(L"Text to cut");
    control->SetSelectionRange(zaf::Range{ 2, 2 });
    //A not editable text box can not cut.
    ASSERT_FALSE(control->Cut());

    control->SetIsEditable(true);
    control->SetSelectionRange(zaf::Range{ 2, 0 });
    //Returns false if nothing is selected.
    ASSERT_FALSE(control->Cut());

    control->SetSelectionRange(zaf::Range{ 2, 4 });
    ASSERT_TRUE(control->Cut());
    ASSERT_EQ(control->Text(), L"Teo cut");
    ASSERT_EQ(zaf::clipboard::Clipboard::GetText(), L"xt t");
    ASSERT_TRUE(control->CanUndo());
}


TEST(TextBoxTest, Paste) {

    zaf::clipboard::Clipboard::SetText(L"text in clipboard");
    auto control = zaf::Create<zaf::TextBox>();
    control->SetIsEditable(false);
    //A not editable text box can not paste.
    ASSERT_FALSE(control->Paste());

    //Can not paste incompatible content.
    auto data_object = zaf::clipboard::DataObject::Create();
    data_object.SetData(
        zaf::clipboard::DataDescriptor::FromFormatType(zaf::clipboard::MakePrivateFormatType(101)),
        zaf::clipboard::Medium::FromString(L""));
    zaf::clipboard::Clipboard::SetDataObject(data_object);
    control->SetIsEditable(true);
    ASSERT_FALSE(control->Paste());

    zaf::clipboard::Clipboard::SetText(L"text in clipboard");
    control->SetText(L"This is a !");
    control->SetSelectionRange(zaf::Range{ 10, 0 });
    ASSERT_TRUE(control->Paste());
    ASSERT_TRUE(control->CanUndo());
    ASSERT_EQ(control->Text(), L"This is a text in clipboard!");
    ASSERT_EQ(control->SelectionRange(), zaf::Range(27, 0));

    //Paste multiple lines to a single line text box.
    control->SetIsMultiline(false);

    zaf::clipboard::Clipboard::SetText(L"line1\r\n line2");
    control->SetSelectionRange(zaf::Range{ 4, 0 });
    ASSERT_TRUE(control->Paste());
    ASSERT_TRUE(control->CanUndo());
    ASSERT_EQ(control->Text(), L"Thisline1 is a text in clipboard!");
    ASSERT_EQ(control->SelectionRange(), zaf::Range(9, 0));
}


TEST(TextBoxTest, PastingEvent) {

    clipboard::Clipboard::SetText(L"PastingEvent");

    auto control = Create<TextBox>();
    control->SetIsEditable(true);

    auto sub = control->PastingEvent().Subscribe([](const textual::PastingInfo& pasting_info) {
        auto text_box = As<TextBox>(pasting_info.Source());
        auto text = pasting_info.DataObject().GetText();
        text_box->SetText(text);
        pasting_info.MarkAsHandled();
    });

    ASSERT_TRUE(control->Paste());
    ASSERT_EQ(control->Text(), L"PastingEvent");
}


TEST(TextBoxTest, InputText) {

    auto control = Create<TextBox>();

    //A not editable text box can not input.
    control->SetIsEditable(false);
    ASSERT_FALSE(control->Input(L"abc"));
    ASSERT_EQ(control->Text(), L"");

    //An editable text box can input.
    control->SetIsEditable(true);
    ASSERT_TRUE(control->Input(L"abc"));
    ASSERT_EQ(control->Text(), L"abc");
    //The operation will be added to the undo history.
    ASSERT_TRUE(control->CanUndo());
    //Input an empty text.
    ASSERT_FALSE(control->Input(L""));

    //Text will be truncated if max length is set.
    control->SetText({});
    control->SetMaxLength(1);
    ASSERT_TRUE(control->Input(L"1122"));
    ASSERT_EQ(control->Text(), L"1");
    ASSERT_FALSE(control->Input(L"33"));
    ASSERT_EQ(control->Text(), L"1");

    //CRLF will not be broken.
    control->SetText({});
    ASSERT_FALSE(control->Input(L"\r\n2"));
    ASSERT_EQ(control->Text(), L"");
    ASSERT_TRUE(control->Input(L"1\r\n2"));
    ASSERT_EQ(control->Text(), L"1");
}


TEST(TextBoxTest, InputStyledText) {

    auto control = Create<TextBox>();

    textual::StyledText styled_text{ L"styled" };

    //A not editable text box can not input.
    control->SetIsEditable(false);
    ASSERT_FALSE(control->Input(styled_text.Clone()));
    ASSERT_EQ(control->Text(), L"");

    //An editable text box can input.
    control->SetIsEditable(true);
    ASSERT_TRUE(control->Input(styled_text.Clone()));
    ASSERT_EQ(control->Text(), L"styled");
    //The operation will be added to the undo history.
    ASSERT_TRUE(control->CanUndo());

    //Input an empty text.
    ASSERT_FALSE(control->Input(L""));
}


TEST(TextBoxTest, InputInlineObject) {

    auto control = Create<TextBox>();

    auto inline_object = Create<InlineObject>();

    //A not editable text box can not input.
    control->SetIsEditable(false);
    ASSERT_FALSE(control->Input(inline_object));
    ASSERT_EQ(control->Text(), L"");

    //An editable text box can input.
    control->SetIsEditable(true);
    ASSERT_TRUE(control->Input(inline_object));
    ASSERT_EQ(control->Text(), L"\ufffc");

    //The object will have a host.
    ASSERT_EQ(inline_object->Host(), control);

    //The operation will be added to the undo history.
    ASSERT_TRUE(control->CanUndo());
}


TEST(TextBoxTest, HitTestAtIndex) {

    auto control = Create<TextBox>();

    //It's legal to hit test at the end of the text.
    auto hit_test_result = control->HitTestAtIndex(0);
    ASSERT_TRUE(hit_test_result.IsText());
    ASSERT_EQ(hit_test_result.Rect().position, zaf::Point{});

    //Throw exception if index is out of range.
    ASSERT_THROW(control->HitTestAtIndex(1), zaf::PreconditionError);
}