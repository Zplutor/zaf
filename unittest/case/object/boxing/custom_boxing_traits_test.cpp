#include <zaf/dynamic/boxing/custom_boxing_traits.h>
#include <zaf/dynamic/object.h>

using namespace zaf;

namespace {

class NoCustomObject { };
class CustomObject { };

template<>
struct zaf__CustomBoxingTraits<CustomObject> {
    using BoxedType = dynamic::Object;
};

}

static_assert(!HasCustomBoxingTraitsV<NoCustomObject>);
static_assert(HasCustomBoxingTraitsV<CustomObject>);