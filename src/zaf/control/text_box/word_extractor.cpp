#include <zaf/control/text_box/word_extractor.h>

namespace zaf::text_box {

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

        //Return the only char at the hit test index if it is not a word char.
        bool hit_test_is_word_char = std::iswalnum(text[hit_test_index]);
        if (!hit_test_is_word_char) {
            return Range(hit_test_index, 1);
        }

        std::size_t word_begin_index = hit_test_index;
        if (hit_test_index > 0) {

            for (auto current = hit_test_index - 1; current >= 0; --current) {

                if (!std::iswalnum(text[current])) {
                    break;
                }

                word_begin_index = current;

                if (current == 0) {
                    break;
                }
            }
        }

        std::size_t word_end_index{};
        for (word_end_index = hit_test_index; word_end_index < text.length(); ++word_end_index) {

            if (!std::iswalnum(text[word_end_index])) {
                break;
            }
        }

        return Range{ word_begin_index, word_end_index - word_begin_index };
    };
}

}