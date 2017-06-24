#include <zaf/control/color_picker.h>
#include <zaf/serialization/properties.h>

namespace zaf {

void ConstantColorPicker::SerializeToDataNode(DataNode& data_node) const {
    data_node.AddChild(property::Color, color_.Serialize());
}


bool ConstantColorPicker::DeserializeFromDataNode(const DataNode& data_node) {

    auto child_node = data_node.GetChild(property::Color);
    if (child_node != nullptr) {
        return color_.Deserialize(*child_node);
    }

    return true;
}


ColorPicker CreateColorPicker(const Color& color) {
    return ConstantColorPicker(color);
}


ZAF_DEFINE_TYPE_NAME(ConstantColorPicker);

}