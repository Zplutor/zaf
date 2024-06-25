#include <zaf/object/boxing/custom_boxed_type.h>
#include <zaf/object/object.h>

using namespace zaf;

namespace {
class NoCustomObject { };
class CustomObject { };
}

template<>
struct GetCustomBoxedType<CustomObject> {
    using type = Object;
};

static_assert(!HasCustomBoxedTypeV<NoCustomObject>);
static_assert(HasCustomBoxedTypeV<CustomObject>);
