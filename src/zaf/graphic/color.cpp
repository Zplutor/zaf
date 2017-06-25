#include <zaf/graphic/color.h>
#include <zaf/base/string/to_numeric.h>
#include <zaf/serialization/data_node.h>
#include <zaf/serialization/properties.h>

namespace zaf {
namespace {

void DecodeARGB(const std::wstring& argb, float& a, float& r, float& g, float& b) {

    a = 0;
    r = 0;
    g = 0;
    b = 0;

    if (argb.length() != 9 && argb.length() != 7) {
        return;
    }

    if (argb[0] != L'#') {
        return;
    }

    std::wstring a_hex;
    std::wstring r_hex;
    std::wstring g_hex;
    std::wstring b_hex;

    bool is_short_notation = argb.length() == 7; 
    if (is_short_notation) {
        a_hex = L"FF";
        r_hex = argb.substr(1, 2);
        g_hex = argb.substr(3, 2);
        b_hex = argb.substr(5, 2);
    }
    else {
        a_hex = argb.substr(1, 2);
        r_hex = argb.substr(3, 2);
        g_hex = argb.substr(5, 2);
        b_hex = argb.substr(7, 2);
    }

    ToNumericOptions options;
    options.Base(16);

    std::uint8_t temp_a = 0;
    if (!TryToNumeric(a_hex, temp_a, options)) {
        return;
    }
    
    std::uint8_t temp_r = 0;
    if (!TryToNumeric(r_hex, temp_r, options)) {
        return;
    }

    std::uint8_t temp_g = 0;
    if (!TryToNumeric(g_hex, temp_g, options)) {
        return;
    }

    std::uint8_t temp_b = 0;
    if (!TryToNumeric(b_hex, temp_b, options)) {
        return;
    }

    float max = std::numeric_limits<std::uint8_t>::max();
    a = temp_a / max;
    r = temp_r / max;
    g = temp_g / max;
    b = temp_b / max;
}

}

void Color::SerializeToDataNode(DataNode& data_node) const {

    data_node.AddChild(property::R, DataNode::CreateNumber(r));
    data_node.AddChild(property::G, DataNode::CreateNumber(g));
    data_node.AddChild(property::B, DataNode::CreateNumber(b));
    data_node.AddChild(property::A, DataNode::CreateNumber(a));
}


bool Color::DeserializeFromDataNode(const DataNode& data_node) {

    data_node.EnumerateKeyedChildren([this](const std::wstring& key, const DataNode& data_node) {
    
        if (key == property::R) {
            r = data_node.GetFloat();
        }
        else if (key == property::G) {
            g = data_node.GetFloat();
        }
        else if (key == property::B) {
            b = data_node.GetFloat();
        }
        else if (key == property::A) {
            a = data_node.GetFloat();
        }
        else if (key == property::ARGB) {
            DecodeARGB(data_node.GetString(), a, r, g, b);
        }
    });

    return true;
}


ZAF_DEFINE_TYPE_NAME(Color);

const Color Color::Black = Color(0, 0, 0);
const Color Color::Blue = Color(0, 0, 1);
const Color Color::Cyan = Color(0, 1, 1);
const Color Color::Gray = Color(0.5, 0.5, 0.5);
const Color Color::Green = Color(0, 0.5, 0);
const Color Color::Lime = Color(0, 1, 0);
const Color Color::Magenta = Color(1, 0, 1);
const Color Color::Red = Color(1, 0, 0);
const Color Color::Transparent = Color(0, 0, 0, 0);
const Color Color::White = Color(1, 1, 1);
const Color Color::Yellow = Color(1, 1, 0);

}