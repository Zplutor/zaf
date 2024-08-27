#include <zaf/control/textual/word_extractor.h>

namespace zaf::textual {
namespace {

bool IsLineBreak(wchar_t ch) {
    return ch == L'\r' || ch == L'\n';
}


std::size_t MoveIndexToLastCharInLineIfNeeded(std::wstring_view text, std::size_t index) {

    if (text.empty()) {
        return 0;
    }

    auto current = index;
    if (current >= text.length()) {

        if (IsLineBreak(text.back())) {
            return text.length();
        }

        current = text.length() - 1;
    }

    if (text[current] == L'\r' && current > 0) {

        if (!IsLineBreak(text[current - 1])) {
            current -= 1;
        }
    }
    else if (text[current] == L'\n') {

        if (current > 0 && text[current - 1] == L'\r') {
            current -= 1;
        }

        if (current > 0 && !IsLineBreak(text[current - 1])) {
            current -= 1;
        }
    }

    return current;
}


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

}

WordExtractor DefaultWordExtractor() noexcept {
    
    return [](std::wstring_view text, std::size_t index) {
    
        auto hit_test_index = MoveIndexToLastCharInLineIfNeeded(text, index);
        if (hit_test_index == text.length()) {
            return Range{ hit_test_index, 0 };
        }

        //Empty line
        if (IsLineBreak(text[hit_test_index])) {
            return Range{ hit_test_index, 0 };
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

        return Range(hit_test_index, 1);
    };
}

}