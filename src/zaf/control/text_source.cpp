#include <zaf/control/text_source.h>

namespace zaf {
namespace {

class DefaultTextSource : public TextSource {
public:
    std::size_t GetTextLength() override {
        return text_.length();
    }

    std::variant<std::wstring_view, std::wstring> GetText() override {
        return std::wstring_view{ text_ };
    }

    TextSourceChangeResult SetText(const std::wstring& text) override {

        if (text_ == text) {
            return {};
        }

        text_ = text;

        TextSourceChangeResult result;
        result.SetIsChanged(true);
        return result;
    }

private:
    std::wstring text_;
};

}

std::unique_ptr<TextSource> TextSource::Default() {
    return std::make_unique<DefaultTextSource>();
}

}