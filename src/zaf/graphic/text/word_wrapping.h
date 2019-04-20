#pragma once

#include <dwrite.h>
#include <optional>
#include <zaf/serialization/serializer.h>

namespace zaf {

/**
 Specifies the word wrapping to be used in a particular multiline paragraph.
 */
enum class WordWrapping {

    /**
     Indicates that words are broken across lines to avoid text overflowing the layout box.
     */
    Wrap = DWRITE_WORD_WRAPPING_WRAP,

    /**
     Indicates that words are kept within the same line even when it overflows the layout box.
     This option is often used with scrolling to reveal overflow text.
     */
    NoWrap = DWRITE_WORD_WRAPPING_NO_WRAP,

    /**
     Words are broken across lines to avoid text overflowing the layout box. Emergency 
     wrapping occurs if the word is larger than the maximum width.

     Available on Windows 8.1 and later only.
     */
    EmergencyBreak = DWRITE_WORD_WRAPPING_EMERGENCY_BREAK,

    /**
     When emergency wrapping, only wrap whole words, never breaking words when the layout
     width is too small for even a single word.

     Available on Windows 8.1 and later only.
     */
    WholeWord = DWRITE_WORD_WRAPPING_WHOLE_WORD,

    /**
     Wrap between any valid character clusters.

     Available on Windows 8.1 and later only.
     */
    Character = DWRITE_WORD_WRAPPING_CHARACTER,
};


std::wstring ConvertWordWrappingToString(WordWrapping word_wrapping);
std::optional<WordWrapping> ConvertWordWrappingFromString(const std::wstring& string);


template<>
class Serializer<WordWrapping> {
public:
    static std::shared_ptr<DataNode> Serialize(WordWrapping word_wrapping) {
        return DataNode::CreateString(ConvertWordWrappingToString(word_wrapping));
    }
};

}