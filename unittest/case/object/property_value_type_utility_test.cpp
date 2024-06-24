#include <gtest/gtest.h>
#include <zaf/object/property_value_type_utility.h>

using namespace zaf;

static_assert(std::is_same_v<MakePropertyValueTypeT<std::int32_t>, Int32>);
static_assert(std::is_same_v<MakePropertyValueTypeT<std::shared_ptr<std::int32_t>>, Int32>);

static_assert(std::is_same_v<MakePropertyValueTypeT<Object>, Object>);
static_assert(std::is_same_v<MakePropertyValueTypeT<std::shared_ptr<Object>> , Object>);

class CustomObject {

};

namespace zaf {

template<>
struct GetCustomPropertyValueType<CustomObject> {
    using type = UInt32;
};

}


using type = MakePropertyValueTypeT<CustomObject>;

//static_assert(std::is_same_v<MakePropertyValueTypeT<CustomObject>, UInt32>);