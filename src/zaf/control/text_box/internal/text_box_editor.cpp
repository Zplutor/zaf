#include <zaf/control/text_box/internal/text_box_editor.h>
#include <cwctype>

namespace zaf::internal {
namespace {

bool ShouldIgnoreChar(wchar_t ch) {

    //Some control chars in ASCII.
    switch (ch) {
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 14:
    case 15:
    case 16:
    case 17:
    case 18:
    case 19:
    case 20:
    case 21:
    case 22:
    case 23:
    case 24:
    case 25:
    case 26:
    case 27: //ESC
    case 28:
    case 29:
    case 30:
    case 31:
        return true;
    default:
        return false;
    }
}

}

TextBoxEditor::TextBoxEditor(TextModel* text_model) : text_model_(text_model) {

}


void TextBoxEditor::HandleKeyDown(const KeyDownInfo& event_info) {
}


void TextBoxEditor::HandleKeyUp(const KeyUpInfo& event_info) {

}


Range TextBoxEditor::HandleCharInput(
    const Range& selection_range,
    const CharInputInfo& event_info) {

    auto ch = event_info.Message().Char();

    //Ignore specific control chars.
    if (ShouldIgnoreChar(ch)) {
        return selection_range;
    }

    //Backspace
    if (ch == '\x8') {
        return HandleBackspace(selection_range);
    }

    std::wstring new_text(1, event_info.Message().Char());
    text_model_->SetTextInRange(new_text, selection_range);
    return Range{ selection_range.index + 1, 0 };
}


Range TextBoxEditor::HandleBackspace(const Range& selection_range) {

    //Remove the selected text.
    if (selection_range.length > 0) {
        text_model_->SetTextInRange({}, selection_range);
        return Range{ selection_range.index, 0 };
    }

    if (selection_range.index == 0) {
        return selection_range;
    }

    //Remove the previous char.
    text_model_->SetTextInRange({}, Range{ selection_range.index - 1, 1 });
    return Range{ selection_range.index - 1, 0 };
}

}