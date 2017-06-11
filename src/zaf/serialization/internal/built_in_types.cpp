#include <zaf/serialization/internal/built_in_types.h>
#include <zaf/control/control.h>
#include <zaf/creation.h>
#include <zaf/graphic/color.h>
#include <zaf/graphic/frame.h>
#include <zaf/graphic/point.h>
#include <zaf/graphic/rect.h>
#include <zaf/graphic/size.h>
#include <zaf/serialization/serializable_type.h>
#include <zaf/serialization/types.h>

namespace zaf {
namespace internal {

template<typename Type>
SerializableTypeProperties CreateProperties(const wchar_t* name) {
    SerializableTypeProperties properties;
    properties.name = name;
    properties.instance_creator = []() { return zaf::Create<Type>(); };
    return properties;
}


std::vector<SerializableTypeProperties> GetBuiltInTypeProperties() {

    return {
        CreateProperties<Color>(type::Color),
        CreateProperties<Control>(type::Control),
        CreateProperties<Frame>(type::Frame),
        CreateProperties<Point>(type::Point),
        CreateProperties<Rect>(type::Rect),
        CreateProperties<Size>(type::Size),
    };
}

}
}