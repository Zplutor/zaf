#include <zaf/graphic/text/text_trimming.h>
#include <zaf/serialization/data_node.h>
#include <zaf/serialization/properties.h>

namespace zaf {
namespace {

const wchar_t* const Names[] = {
    L"None",
    L"Character",
    L"Word",
};

}

void TextTrimming::SerializeToDataNode(DataNode& data_node) const {

    data_node.AddChild(property::Granularity, DataNode::CreateString(ConvertTextTrimmingGranularityToString(granularity)));
    data_node.AddChild(property::Delimiter, DataNode::CreateString(std::wstring(1, delimiter)));
    data_node.AddChild(property::DelimiterCount, DataNode::CreateNumber(delimiter_count));
}


bool TextTrimming::DeserializeFromDataNode(const DataNode& data_node) {

    data_node.EnumerateKeyedChildren([this](const std::wstring& key, const DataNode& data_node) {

        if (key == property::Granularity) {
            auto granularity = ConvertTextTrimmingGranularityFromString(data_node.GetString());
            this->granularity = granularity.has_value() ? granularity.value() : Granularity::None;
        }
        else if (key == property::Delimiter) {
            auto string = data_node.GetString();
            delimiter = string.length() == 1 ? string.front() : 0;
        }
        else if (key == property::DelimiterCount) {
            delimiter_count = data_node.GetUint32();
        }
    });

    return true;
}


std::wstring ConvertTextTrimmingGranularityToString(TextTrimming::Granularity granularity) {
    return Names[static_cast<std::size_t>(granularity)];
}


zaf::optional<TextTrimming::Granularity> ConvertTextTrimmingGranularityFromString(const std::wstring& string) {
    
    for (std::size_t index = 0; index < sizeof(Names) / sizeof(const wchar_t*); ++index) {
        if (string == Names[index]) {
            return static_cast<TextTrimming::Granularity>(index);
        }
    }

    return {};
}


ZAF_DEFINE_TYPE_NAME(TextTrimming);

}