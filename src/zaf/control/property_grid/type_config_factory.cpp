#include <zaf/control/property_grid/type_config_factory.h>
#include <zaf/base/container/utility/find.h>
#include <zaf/control/control.h>
#include <zaf/control/property_grid/bool_config.h>
#include <zaf/control/property_grid/color_config.h>
#include <zaf/control/property_grid/control_config.h>
#include <zaf/control/property_grid/parsable_type_config.h>
#include <zaf/control/property_grid/enum_config.h>
#include <zaf/control/property_grid/frame_config.h>
#include <zaf/control/property_grid/point_config.h>
#include <zaf/control/property_grid/rect_config.h>
#include <zaf/control/property_grid/size_config.h>
#include <zaf/control/property_grid/textual_control_config.h>
#include <zaf/control/textual_control.h>
#include <zaf/graphic/color.h>
#include <zaf/graphic/font_weight.h>
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
        { Boolean::StaticType(), ConfigGetter<BoolConfig> },
        { Color::StaticType(), ConfigGetter<ColorConfig> },
        { Control::StaticType(), ConfigGetter<ControlConfig> },
        { TextualControl::StaticType(), ConfigGetter<TextualControlConfig> },
        { Frame::StaticType(), ConfigGetter<FrameConfig> },
        { Rect::StaticType(), ConfigGetter<RectConfig> },
        { Point::StaticType(), ConfigGetter<PointConfig> },
        { Size::StaticType(), ConfigGetter<SizeConfig> },
        { Int8::StaticType(), ConfigGetter<ParsableTypeConfig> },
        { UInt8::StaticType(), ConfigGetter<ParsableTypeConfig> },
        { Int16::StaticType(), ConfigGetter<ParsableTypeConfig> },
        { UInt16::StaticType(), ConfigGetter<ParsableTypeConfig> },
        { Int32::StaticType(), ConfigGetter<ParsableTypeConfig> },
        { UInt32::StaticType(), ConfigGetter<ParsableTypeConfig> },
        { Int64::StaticType(), ConfigGetter<ParsableTypeConfig> },
        { UInt64::StaticType(), ConfigGetter<ParsableTypeConfig> },
        { Float::StaticType(), ConfigGetter<ParsableTypeConfig> },
        { Double::StaticType(), ConfigGetter<ParsableTypeConfig> },
        { WideString::StaticType(), ConfigGetter<ParsableTypeConfig> },
        { FontWeight::StaticType(), ConfigGetter<ParsableTypeConfig> },
    };

    auto current_type = type;
    while (current_type) {

        auto getter = Find(config_map, current_type);
        if (getter) {
            return (*getter)();
        }

        current_type = current_type->BaseType();
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