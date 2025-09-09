#include <zaf/control/property_grid_delegate.h>
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
#include <zaf/dynamic/boxing/boolean.h>
#include <zaf/dynamic/boxing/numeric.h>
#include <zaf/dynamic/boxing/string.h>
#include <zaf/dynamic/enum_type.h>

using namespace zaf::property_grid;

namespace zaf {
namespace {

template<typename T>
std::shared_ptr<TypeConfig> MakeConfig() {
    static auto config = std::make_shared<T>();
    return config;
}


std::shared_ptr<TypeConfig> GetObjectTypeConfig(dynamic::ObjectType* type) {

    static const std::map<dynamic::ObjectType*, std::shared_ptr<TypeConfig>(*)()> config_map{
        { Boolean::StaticType(), MakeConfig<BoolConfig> },
        { Color::StaticType(), MakeConfig<ColorConfig> },
        { Control::StaticType(), MakeConfig<ControlConfig> },
        { TextualControl::StaticType(), MakeConfig<TextualControlConfig> },
        { Frame::StaticType(), MakeConfig<FrameConfig> },
        { Rect::StaticType(), MakeConfig<RectConfig> },
        { Point::StaticType(), MakeConfig<PointConfig> },
        { Size::StaticType(), MakeConfig<SizeConfig> },
        { Int8::StaticType(), MakeConfig<ParsableTypeConfig> },
        { UInt8::StaticType(), MakeConfig<ParsableTypeConfig> },
        { Int16::StaticType(), MakeConfig<ParsableTypeConfig> },
        { UInt16::StaticType(), MakeConfig<ParsableTypeConfig> },
        { Int32::StaticType(), MakeConfig<ParsableTypeConfig> },
        { UInt32::StaticType(), MakeConfig<ParsableTypeConfig> },
        { Int64::StaticType(), MakeConfig<ParsableTypeConfig> },
        { UInt64::StaticType(), MakeConfig<ParsableTypeConfig> },
        { Float::StaticType(), MakeConfig<ParsableTypeConfig> },
        { Double::StaticType(), MakeConfig<ParsableTypeConfig> },
        { WideString::StaticType(), MakeConfig<ParsableTypeConfig> },
        { FontWeight::StaticType(), MakeConfig<ParsableTypeConfig> },
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


std::shared_ptr<TypeConfig> GetEnumTypeConfig(dynamic::ObjectType* type) {

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

std::shared_ptr<property_grid::TypeConfig> PropertyGridDelegate::GetTypeConfig(
    dynamic::ObjectType* type) {

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