#include <zaf/control/textual/word_extractor.h>

namespace zaf::textual {
namespace {

std::optional<Range> ExtendIndexToWordRange(
    std::wstring_view text,
    std::size_t index,
    const std::function<bool(wchar_t)>& predictor) {

    if (!predictor(text[index])) {
        return std::nullopt;
    }

    std::size_t word_begin_index = index;
    if (index > 0) {

        for (auto current = index - 1; current >= 0; --current) {

            if (!predictor(text[current])) {
                break;
            }

            word_begin_index = current;

            if (current == 0) {
                break;
            }
        }
    }

    std::size_t word_end_index{};
    for (word_end_index = index; word_end_index < text.length(); ++word_end_index) {

        if (!predictor(text[word_end_index])) {
            break;
        }
    }

    return Range{ word_begin_index, word_end_index - word_begin_index };
}


std::optional<Range> ExtendIndexToLineBreakRange(std::wstring_view text, std::size_t index) {

    if (text[index] == L'\r') {

        std::size_t next_index = index + 1;
        if (next_index < text.length() && text[next_index] == L'\n') {
            return Range{ index, 2 };
        }
        else {
            return Range{ index, 1 };
        }
    }

    if (text[index] == L'\n') {

        if (index > 0 && text[index - 1] == L'\r') {
            return Range{ index - 1, 2 };
        }
        else {
            return Range{ index, 1 };
        }
    }

    return std::nullopt;
}

}

WordExtractor DefaultWordExtractor() noexcept {
    
    return [](std::wstring_view text, std::size_t index) {
    
        if (text.empty()) {
            return Range{};
        }

        //Locate to the last char if the specified index is beyond the last char.
        std::size_t hit_test_index = index;
        if (hit_test_index >= text.length()) {
            hit_test_index = text.length() - 1;
        }

        //Alpha and numbers
        auto range = ExtendIndexToWordRange(text, hit_test_index, std::iswalnum);
        if (range) {
            return *range;
        }

        //Blank chars
        range = ExtendIndexToWordRange(text, hit_test_index, std::iswblank);
        if (range) {
            return *range;
        }

        //Line breaks
        range = ExtendIndexToLineBreakRange(text, hit_test_index);
        if (range) {
            return *range;
        }

        return Range(hit_test_index, 1);
    };
}

}