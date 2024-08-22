#include <zaf/internal/textual/line_utility.h>

namespace zaf::internal {
namespace {

std::wstring_view GetLineBreakString(textual::LineBreak line_break) noexcept {
    switch (line_break) {
    case textual::LineBreak::CRLF:
        return L"\r\n";
    case textual::LineBreak::CR:
        return L"\r";
    case textual::LineBreak::LF:
        return L"\n";
    default:
        return L"";
    }
}

class TextShim {
public:
    explicit TextShim(std::wstring& text) : text_(text) {

    }

    std::size_t Length() const noexcept {
        return text_.length();
    }

    wchar_t At(std::size_t index) const noexcept {
        return text_[index];
    }

    std::size_t FindFirstOf(std::wstring_view sub_string, std::size_t start_index) const {
        return text_.find_first_of(sub_string, start_index);
    }

    std::wstring_view SubString(std::size_t start_index, std::size_t length) const {
        return { text_.data() + start_index, length };
    }

    void Erase(std::size_t start_index) {
        text_.erase(start_index);
    }

    void Replace(std::size_t start_index, std::size_t length, std::wstring_view new_string) {
        text_.replace(start_index, length, new_string);
    }

private:
    std::wstring& text_;
};


class StyledTextShim {
public:
    explicit StyledTextShim(textual::StyledText& styled_text) : styled_text_(styled_text) {

    }

    std::size_t Length() const noexcept {
        return styled_text_.Length();
    }

    wchar_t At(std::size_t index) const noexcept {
        return styled_text_.Text()[index];
    }

    std::size_t FindFirstOf(std::wstring_view sub_string, std::size_t start_index) const {
        return styled_text_.Text().find_first_of(sub_string, start_index);
    }

    std::wstring_view SubString(std::size_t start_index, std::size_t length) const {
        return { styled_text_.Text().data() + start_index, length };
    }

    void Erase(std::size_t start_index) {
        styled_text_.SetTextInRange({}, { start_index, styled_text_.Length() - start_index });
    }

    void Replace(std::size_t start_index, std::size_t length, std::wstring_view new_string) {
        styled_text_.SetTextInRange(new_string, { start_index, length });
    }

private:
    textual::StyledText& styled_text_;
};

template<typename T>
class Reviser {
public:
    static void ReviseLines(T text, bool supports_multiline, textual::LineBreak line_break) {

        if (!supports_multiline) {
            RemoveRedundantLines(text);
            return;
        }

        if (line_break != textual::LineBreak::Unspecific) {
            ReplaceLineBreaks(text, line_break);
        }
    }

private:
    static void RemoveRedundantLines(T text) {

        auto line_break_index = text.FindFirstOf(L"\r\n", 0);
        if (line_break_index != std::wstring::npos) {
            text.Erase(line_break_index);
        }
    }

    static void ReplaceLineBreaks(T text, textual::LineBreak new_line_break) {

        auto new_line_break_string = GetLineBreakString(new_line_break);

        std::size_t current_index{};
        while (current_index < text.Length()) {

            auto line_break_index = text.FindFirstOf(L"\r\n", current_index);
            if (line_break_index == std::wstring::npos) {
                break;
            }

            std::size_t line_break_length{};
            if (text.At(line_break_index) == L'\r' && text.At(line_break_index + 1) == L'\n') {
                line_break_length = 2;
            }
            else {
                line_break_length = 1;
            }

            auto old_line_break_string = text.SubString(line_break_index, line_break_length);
            if (old_line_break_string != new_line_break_string) {
                text.Replace(line_break_index, line_break_length, new_line_break_string);
            }

            current_index = line_break_index + new_line_break_string.length();
        }
    }
};

}


void LineUtility::ReviseLinesInStyledText(
    textual::StyledText& styled_text, 
    bool supports_multiline,
    textual::LineBreak line_break) {

    Reviser<StyledTextShim>::ReviseLines(
        StyledTextShim{ styled_text },
        supports_multiline, 
        line_break);
}


void LineUtility::ReviseLinesInText(
    std::wstring& text, 
    bool supports_multiline, 
    textual::LineBreak line_break) {

    Reviser<TextShim>::ReviseLines(TextShim{ text }, supports_multiline, line_break);
}

}