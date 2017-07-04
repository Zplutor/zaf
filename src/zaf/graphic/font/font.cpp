#include <zaf/graphic/font/font.h>
#include <zaf/application.h>
#include <zaf/serialization/data_node.h>
#include <zaf/serialization/properties.h>

namespace zaf {
namespace {

int ConvertFontWeightFromString(const std::wstring& string) {

    std::pair<const wchar_t*, int> map[] = {
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
        if (each_pair.first == string) {
            return each_pair.second;
        }
    }

    return FontWeight::Regular;
}

}

const Font Font::GetDefault() {

    LOGFONT logfont = { 0 };
    SystemParametersInfo(SPI_GETICONTITLELOGFONT, sizeof(logfont), &logfont, 0);
    return Font::FromLOGFONT(logfont);
}


const Font Font::FromLOGFONT(const LOGFONT& logfont) {

    Font font;
    font.family_name = logfont.lfFaceName;

    font.weight = logfont.lfWeight;
    if (font.weight == 0) {
        font.weight = FontWeight::Regular;
    }
    else if (font.weight == 1000) {
        font.weight = FontWeight::Maximum;
    }
    
    float height = static_cast<float>(std::abs(logfont.lfHeight));
    float y_dpi = Application::GetInstance().GetDpi().second;
    if (y_dpi != 0) {
        font.size = height * 96 / y_dpi;
    }
    else {
        font.size = height;
    }

    return font;
}


void Font::SerializeToDataNode(DataNode& data_node) const {

    data_node.AddChild(property::FamilyName, DataNode::CreateString(family_name));
    data_node.AddChild(property::Size, DataNode::CreateNumber(size));
    data_node.AddChild(property::Weight, DataNode::CreateNumber(weight));
}


bool Font::DeserializeFromDataNode(const DataNode& data_node) {

    data_node.EnumerateKeyedChildren([this](const std::wstring& key, const DataNode& data_node) {
    
        if (key == property::FamilyName) {
            family_name = data_node.GetString();
        }
        else if (key == property::Size) {
            size = data_node.GetFloat();
        }
        else if (key == property::Weight) {
            if (data_node.IsString()) {
                weight = ConvertFontWeightFromString(data_node.GetString());
            }
            else {
                weight = data_node.GetInt32();
                if (weight < FontWeight::Minimum || FontWeight::Maximum < weight) {
                    weight = FontWeight::Regular;
                }
            }
        }
    });

    return true;
}


ZAF_DEFINE_TYPE_NAME(Font);

}