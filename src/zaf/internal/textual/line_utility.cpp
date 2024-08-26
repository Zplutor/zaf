#include <zaf/internal/textual/line_utility.h>

using namespace zaf::textual;

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


class TextViewShim {
public:
    explicit TextViewShim(std::wstring_view text_view) : text_(text_view) {

    }

    std::size_t Length() const noexcept {

        return std::visit([](const auto& text) {

            using TextType = std::decay_t<decltype(text)>;
            if constexpr (std::is_same_v<TextType, std::wstring_view>) {
                return text.length();
            }

            if constexpr (std::is_same_v<TextType, std::wstring>) {
                return text.length();
            }
        }, 
        text_);
    }

    wchar_t At(std::size_t index) const noexcept {

        return std::visit([index](const auto& text) {

            using TextType = std::decay_t<decltype(text)>;
            if constexpr (std::is_same_v<TextType, std::wstring_view>) {
                return text[index];
            }

            if constexpr (std::is_same_v<TextType, std::wstring>) {
                return text[index];
            }
        },
        text_);
    }

    std::size_t FindFirstOf(std::wstring_view sub_string, std::size_t start_index) const {
        
        return std::visit([sub_string, start_index](const auto& text) {

            using TextType = std::decay_t<decltype(text)>;
            if constexpr (std::is_same_v<TextType, std::wstring_view>) {
                return text.find_first_of(sub_string, start_index);
            }

            if constexpr (std::is_same_v<TextType, std::wstring>) {
                return text.find_first_of(sub_string, start_index);
            }
        },
        text_);
    }

    std::wstring_view SubString(std::size_t start_index, std::size_t length) const {

        return std::visit([start_index, length](const auto& text) {

            using TextType = std::decay_t<decltype(text)>;
            if constexpr (std::is_same_v<TextType, std::wstring_view>) {
                return text.substr(start_index, length);
            }

            if constexpr (std::is_same_v<TextType, std::wstring>) {
                return std::wstring_view{ text.data() + start_index, length };
            }
        },
        text_);
    }

    void Erase(std::size_t start_index) {
        
        std::visit([start_index](auto& text) {

            using TextType = std::decay_t<decltype(text)>;
            if constexpr (std::is_same_v<TextType, std::wstring_view>) {
                text = text.substr(0, start_index);
            }

            if constexpr (std::is_same_v<TextType, std::wstring>) {
                text.erase(start_index);
            }
        },
        text_);
    }

    void Replace(std::size_t start_index, std::size_t length, std::wstring_view new_string) {

        auto string_view = std::get_if<std::wstring_view>(&text_);
        if (string_view) {

            std::wstring copied_string;
            copied_string.reserve(string_view->length());

            copied_string.append(string_view->substr(0, start_index));
            copied_string.append(new_string);
            copied_string.append(string_view->substr(start_index + length));

            text_ = std::move(copied_string);
        }
        else {
            auto& string = std::get<std::wstring>(text_);
            string.replace(start_index, length, new_string);
        }
    }

    std::variant<std::wstring_view, std::wstring> Detach() {
        return std::move(text_);
    }

private:
    std::variant<std::wstring_view, std::wstring> text_;
};


class StyledTextShim {
public:
    explicit StyledTextShim(StyledText& styled_text) : styled_text_(styled_text) {

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
    StyledText& styled_text_;
};


class StyledTextViewShim {
public:
    explicit StyledTextViewShim(const StyledText& styled_text_view) : 
        styled_text_(std::ref(styled_text_view)) {

    }

    std::size_t Length() const noexcept {

        return std::visit([](const auto& text) {

            using StyledTextType = std::decay_t<decltype(text)>;
            if constexpr (std::is_same_v<StyledTextType, 
                          std::reference_wrapper<const StyledText>>) {
                return text.get().Length();
            }

            if constexpr (std::is_same_v<StyledTextType, StyledText>) {
                return text.Length();
            }
        }, 
        styled_text_);
    }

    wchar_t At(std::size_t index) const noexcept {
        
        return std::visit([index](const auto& text) {

            using StyledTextType = std::decay_t<decltype(text)>;
            if constexpr (std::is_same_v<StyledTextType,
                std::reference_wrapper<const StyledText>>) {
                return text.get().Text()[index];
            }

            if constexpr (std::is_same_v<StyledTextType, StyledText>) {
                return text.Text()[index];
            }
        },
        styled_text_);
    }

    std::size_t FindFirstOf(std::wstring_view sub_string, std::size_t start_index) const {
        
        return std::visit([sub_string, start_index](const auto& text) {

            using StyledTextType = std::decay_t<decltype(text)>;
            if constexpr (std::is_same_v<StyledTextType,
                          std::reference_wrapper<const StyledText>>) {
                return text.get().Text().find_first_of(sub_string, start_index);
            }

            if constexpr (std::is_same_v<StyledTextType, StyledText>) {
                return text.Text().find_first_of(sub_string, start_index);
            }
        },
        styled_text_);
    }

    std::wstring_view SubString(std::size_t start_index, std::size_t length) const {
        
        return std::visit([start_index, length](const auto& text) {

            using StyledTextType = std::decay_t<decltype(text)>;
            if constexpr (std::is_same_v<StyledTextType,
                          std::reference_wrapper<const StyledText>>) {
                return std::wstring_view{ text.get().Text().data() + start_index, length };
            }

            if constexpr (std::is_same_v<StyledTextType, StyledText>) {
                return std::wstring_view{ text.Text().data() + start_index, length };
            }
        },
        styled_text_);
    }

    void Erase(std::size_t start_index) {
        
        auto ref = std::get_if<std::reference_wrapper<const StyledText>>(&styled_text_);
        if (ref) {
            auto copied_styled_text = ref->get().GetSubText({ 0, start_index });
            styled_text_ = std::move(copied_styled_text);
        }
        else {
            auto& styled_text = std::get<StyledText>(styled_text_);
            styled_text.SetTextInRange({}, { start_index, styled_text.Length() - start_index });
        }
    }

    void Replace(std::size_t start_index, std::size_t length, std::wstring_view new_string) {
        
        auto ref = std::get_if<std::reference_wrapper<const StyledText>>(&styled_text_);
        if (ref) {
            auto copied_styled_text = ref->get().Clone();
            copied_styled_text.SetTextInRange(new_string, { start_index, length });
            styled_text_ = std::move(copied_styled_text);
        }
        else {
            auto& styled_text = std::get<StyledText>(styled_text_);
            styled_text.SetTextInRange(new_string, { start_index, length });
        }
    }

    std::variant<std::reference_wrapper<const StyledText>, StyledText> Detach() {
        return std::move(styled_text_);
    }

private:
    std::variant<std::reference_wrapper<const StyledText>, StyledText> styled_text_;
};


template<typename T>
class Reviser {
public:
    static bool ReviseLines(
        T& text,
        bool supports_multiline, 
        LineBreak line_break,
        const RangeReplacedCallback& range_replaced_callback) {

        if (!supports_multiline) {
            return RemoveRedundantLines(text, range_replaced_callback);
        }

        if (line_break != LineBreak::Unspecific) {
            return ReplaceLineBreaks(text, line_break, range_replaced_callback);
        }

        return false;
    }

private:
    static bool RemoveRedundantLines(
        T& text,
        const RangeReplacedCallback& range_replaced_callback) {

        auto line_break_index = text.FindFirstOf(L"\r\n", 0);
        if (line_break_index == std::wstring::npos) {
            return false;
        }

        auto length = text.Length();
        text.Erase(line_break_index);

        if (range_replaced_callback) {
            range_replaced_callback({ line_break_index, length - line_break_index }, 0);
        }
        return true;
    }

    static bool ReplaceLineBreaks(
        T& text,
        LineBreak new_line_break,
        const RangeReplacedCallback& range_replaced_callback) {

        auto new_line_break_string = GetLineBreakString(new_line_break);

        bool has_replaced{};
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

                if (range_replaced_callback) {
                    range_replaced_callback(
                        { line_break_index, line_break_length },
                        new_line_break_string.length());
                }
                has_replaced = true;
            }

            current_index = line_break_index + new_line_break_string.length();
        }

        return has_replaced;
    }
};

}


bool ReviseLinesInStyledText(
    StyledText& styled_text, 
    bool supports_multiline,
    LineBreak line_break,
    const RangeReplacedCallback& range_replaced_callback) {

    StyledTextShim styled_text_shim{ styled_text };
    return Reviser<StyledTextShim>::ReviseLines(
        styled_text_shim,
        supports_multiline,
        line_break,
        range_replaced_callback);
}


std::variant<std::reference_wrapper<const StyledText>, StyledText> 
    ReviseLinesInStyledTextView(
        const StyledText& styled_text_view,
        bool supports_multiline,
        LineBreak line_break) {

    StyledTextViewShim styled_text_view_shim{ styled_text_view };
    Reviser<StyledTextViewShim>::ReviseLines(
        styled_text_view_shim,
        supports_multiline,
        line_break,
        nullptr);

    return styled_text_view_shim.Detach();
}


void ReviseLinesInText(
    std::wstring& text, 
    bool supports_multiline, 
    LineBreak line_break) {

    TextShim text_shim{ text };
    Reviser<TextShim>::ReviseLines(text_shim, supports_multiline, line_break, nullptr);
}


std::variant<std::wstring_view, std::wstring> ReviseLinesInTextView(
    std::wstring_view text_view, 
    bool supports_multiline,
    LineBreak line_break) {

    TextViewShim text_view_shim{ text_view };
    Reviser<TextViewShim>::ReviseLines(text_view_shim, supports_multiline, line_break, nullptr);
    return text_view_shim.Detach();
}

}