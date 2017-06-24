#include <zaf/graphic/color.h>
#include <zaf/serialization/data_node.h>
#include <zaf/serialization/properties.h>

namespace zaf {

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