#include <zaf/control/property_grid/type_config_factory.h>
#include <zaf/base/container/utility/find.h>
#include <zaf/control/control.h>
#include <zaf/control/property_grid/bool_config.h>
#include <zaf/control/property_grid/color_config.h>
#include <zaf/control/property_grid/control_config.h>
#include <zaf/control/property_grid/editable_type_config.h>
#include <zaf/control/property_grid/enum_config.h>
#include <zaf/control/property_grid/frame_config.h>
#include <zaf/control/property_grid/point_config.h>
#include <zaf/control/property_grid/rect_config.h>
#include <zaf/control/property_grid/size_config.h>
#include <zaf/control/property_grid/textual_control_config.h>
#include <zaf/control/textual_control.h>
#include <zaf/graphic/color.h>
#include <zaf/graphic/font/font_weight.h>
#include <zaf/graphic/frame.h>
#include <zaf/graphic/rect.h>
#include <zaf/graphic/point.h>
#include <zaf/graphic/size.h>
#include <zaf/object/boxing/boolean.h>
#include <zaf/object/boxing/numeric.h>
#include <zaf/object/boxing/string.h>
#include <zaf/object/enum_type.h>

namespace zaf::property_grid {
namespace {

template<typename T>
std::shared_ptr<TypeConfig> ConfigGetter() {
    static auto config = std::make_shared<T>();
    return config;
}


std::shared_ptr<TypeConfig> GetObjectTypeConfig(ObjectType* type) {

    static const std::map<ObjectType*, std::shared_ptr<TypeConfig>(*)()> config_map{
        { Boolean::Type, ConfigGetter<BoolConfig> },
        { Color::Type, ConfigGetter<ColorConfig> },
        { Control::Type, ConfigGetter<ControlConfig> },
        { TextualControl::Type, ConfigGetter<TextualControlConfig> },
        { Frame::Type, ConfigGetter<FrameConfig> },
        { Rect::Type, ConfigGetter<RectConfig> },
        { Point::Type, ConfigGetter<PointConfig> },
        { Size::Type, ConfigGetter<SizeConfig> },
        { Int8::Type, ConfigGetter<EditableTypeConfig> },
        { UInt8::Type, ConfigGetter<EditableTypeConfig> },
        { Int16::Type, ConfigGetter<EditableTypeConfig> },
        { UInt16::Type, ConfigGetter<EditableTypeConfig> },
        { Int32::Type, ConfigGetter<EditableTypeConfig> },
        { UInt32::Type, ConfigGetter<EditableTypeConfig> },
        { Int64::Type, ConfigGetter<EditableTypeConfig> },
        { UInt64::Type, ConfigGetter<EditableTypeConfig> },
        { Float::Type, ConfigGetter<EditableTypeConfig> },
        { Double::Type, ConfigGetter<EditableTypeConfig> },
        { WideString::Type, ConfigGetter<EditableTypeConfig> },
        { FontWeight::Type, ConfigGetter<EditableTypeConfig> },
    };

    auto current_type = type;
    while (current_type) {

        auto getter = Find(config_map, current_type);
        if (getter) {
            return (*getter)();
        }

        current_type = current_type->GetBase();
    }

    return nullptr;
}


std::shared_ptr<TypeConfig> GetEnumTypeConfig(ObjectType* type) {

    auto enum_type = dynamic_cast<EnumType*>(type);
    if (!enum_type) {
        return nullptr;
    }

    static auto enum_config = std::make_shared<EnumConfig>();
    return enum_config;
}


std::shared_ptr<TypeConfig> GetDefaultConfig() {
    static auto default_config = std::make_shared<TypeConfig>();
    return default_config;
}

}

std::shared_ptr<TypeConfig> TypeConfigFactory::GetConfig(ObjectType* type) {

    auto result = GetObjectTypeConfig(type);
    if (result) {
        return result;
    }

    result = GetEnumTypeConfig(type);
    if (result) {
        return result;
    }

    return GetDefaultConfig();
}

}