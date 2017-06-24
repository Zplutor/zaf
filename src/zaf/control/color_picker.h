#pragma once

#include <functional>
#include <zaf/graphic/color.h>
#include <zaf/serialization/serializer.h>

namespace zaf {

class Control;

/**
 The prorotype of color picker.

 @param control
    The control that getting the color.

 A color picker is called by controls to get colors.
 */
typedef std::function<Color(const Control& control)> ColorPicker;

/**
 Represents a color picker that has constant color.

 Typically, you should use CreateColorPicker function to create a 
 constant color picker.
 */
class ConstantColorPicker : public SerializableObject {
public:
    ZAF_DECLARE_TYPE_NAME();

public:
    ConstantColorPicker() { }
    ConstantColorPicker(const Color& color) : color_(color) { }

    Color operator()(const Control&) {
        return GetColor();
    }

    const Color GetColor() const {
        return color_;
    }

    void SerializeToDataNode(DataNode& data_node) const override;
    bool DeserializeFromDataNode(const DataNode& data_node) override;

private:
    Color color_;
};

/**
 Create a constant color picker that returns the specified color.
 */
ColorPicker CreateColorPicker(const Color& color);

template<>
class Serializer<ColorPicker> {
public:
    static std::shared_ptr<DataNode> Serialize(const ColorPicker& color_picker) {

        auto constant_color_picker = color_picker.target<ConstantColorPicker>();
        if (constant_color_picker != nullptr) {
            return constant_color_picker->Serialize();
        }
        return nullptr;
    }
};

}