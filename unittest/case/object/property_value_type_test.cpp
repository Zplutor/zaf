#include <zaf/object/property_value_type.h>

using namespace zaf;

static_assert(std::is_same_v<MakePropertyValueTypeT<std::int32_t>, Int32>);
static_assert(std::is_same_v<MakePropertyValueTypeT<std::shared_ptr<std::int32_t>>, Int32>);

static_assert(std::is_same_v<MakePropertyValueTypeT<Object>, Object>);
static_assert(std::is_same_v<MakePropertyValueTypeT<std::shared_ptr<Object>> , Object>);

namespace {
class CustomObject { };
}

namespace zaf {

template<>
struct CustomPropertyValueTypeHandler<CustomObject> {
    using BoxedType = UInt32;
};

}

static_assert(std::is_same_v<MakePropertyValueTypeT<CustomObject>, UInt32>);