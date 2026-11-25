#include <zaf/core/object/boxing/custom_boxing_traits.h>
#include <zaf/core/object/object.h>

using namespace zaf;

namespace {

class NoCustomObject { };
class CustomObject { };

template<>
struct zaf__CustomBoxingTraits<CustomObject> {
    using BoxedType = Object;
};

}

static_assert(!HasCustomBoxingTraitsV<NoCustomObject>);
static_assert(HasCustomBoxingTraitsV<CustomObject>);