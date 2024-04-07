#include <zaf/internal/textual/text_box_index_manager.h>
#include <zaf/base/error/contract.h>
#include <zaf/internal/textual/text_box_module_context.h>
#include <zaf/internal/textual/text_model.h>

namespace zaf::internal {

TextBoxIndexManager::TextBoxIndexManager(TextBoxModuleContext* context) : TextBoxModule(context) {

}


void TextBoxIndexManager::Initialize() {

}


std::size_t TextBoxIndexManager::GetPreviousIndex(std::size_t index) const {

    auto& text_model = Context().TextModel();
    std::wstring_view text = text_model.GetText();
    ZAF_EXPECT(index <= text.length());

    if (index == 0) {
        return index;
    }

    auto previous_index = index - 1;
    if (previous_index == 0) {
        return previous_index;
    }

    //Move index to the beginning of the inline object if the index is inside an inline object.
    auto inline_objects = Context().TextModel().StyledText().InlineObjects();
    auto iterator = inline_objects.FindItemContainsIndex(previous_index);
    if (iterator != inline_objects.end()) {

        previous_index = iterator->Range().index;
    }
    else {

        //Skip CRLF line break.
        if (text[previous_index - 1] == L'\r' && text[previous_index] == L'\n') {
            --previous_index;
        }
    }

    return previous_index;
}


std::size_t TextBoxIndexManager::GetNextIndex(std::size_t index) const {
    
    std::wstring_view text = Context().TextModel().GetText();
    ZAF_EXPECT(index <= text.length());

    if (index == text.length()) {
        return index;
    }

    //Move index to the end of the inline object if the index is inside an inline object.
    auto inline_objects = Context().TextModel().StyledText().InlineObjects();
    auto iterator = inline_objects.FindItemContainsIndex(index);
    if (iterator != inline_objects.end()) {

        return iterator->Range().EndIndex();
    }

    auto next_index = index + 1;
    if (next_index == text.length()) {
        return next_index;
    }

    //Skip CRLF line break.
    if (text[index] == L'\r' && text[next_index] == L'\n') {
        ++next_index;
    }

    return next_index;
}

}