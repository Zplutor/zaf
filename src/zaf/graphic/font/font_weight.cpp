#include <zaf/graphic/font/font_weight.h>
#include <zaf/base/as.h>
#include <zaf/base/error/basic_error.h>
#include <zaf/base/string/to_numeric.h>
#include <zaf/object/parsing/object_parser.h>
#include <zaf/object/parsing/xaml_utility.h>
#include <zaf/object/type_definition.h>

namespace zaf {
namespace {

class FontWeightParser : public ObjectParser {
public:
    void ParseFromAttribute(const std::wstring& attribute_value, Object& object) override {
        Parse(attribute_value, object);
    }

    void ParseFromNode(const XamlNode& node, Object& object) override {

        __super::ParseFromNode(node, object);

        auto content_string = GetContentStringFromXamlNode(node);
        if (content_string) {
            Parse(*content_string, object);
        }
        else {
            ZAF_THROW_ERRC(BasicErrc::InvalidValue);
        }
    }

private:
    void Parse(const std::wstring& string_value, Object& object) {

        int value = ParseValue(string_value);
        As<FontWeight>(object) = value;
    }

    int ParseValue(const std::wstring& value) {

        int weight{};
        if (TryToNumeric(value, weight)) {
            return weight;
        }

        static constexpr std::pair<const wchar_t*, int> map[] = {
            { L"Black", FontWeight::Black },
            { L"Bold", FontWeight::Bold },
            { L"ExtraBlack", FontWeight::ExtraBlack },
            { L"ExtraBold", FontWeight::ExtraBold },
            { L"ExtraLight", FontWeight::ExtraLight },
            { L"Light", FontWeight::Light },
            { L"Maximum", FontWeight::Maximum },
            { L"Medium", FontWeight::Medium },
            { L"Minimum", FontWeight::Minimum },
            { L"Regular", FontWeight::Regular },
            { L"SemiBold", FontWeight::SemiBold },
            { L"SemiLight", FontWeight::SemiLight },
            { L"Thin", FontWeight::Thin },
        };

        for (const auto& each_pair : map) {
            if (each_pair.first == value) {
                return each_pair.second;
            }
        }

        ZAF_THROW_ERRC(BasicErrc::InvalidValue);
    }
};

}

ZAF_DEFINE_TYPE(FontWeight)
ZAF_DEFINE_TYPE_PARSER(FontWeightParser)
ZAF_DEFINE_TYPE_END

ZAF_DEFINE_EQUALITY(FontWeight)

std::wstring FontWeight::ToString() const {
    return std::to_wstring(value_);
}

}