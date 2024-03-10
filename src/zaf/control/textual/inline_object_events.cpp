#include <zaf/control/textual/inline_object_events.h>
#include <zaf/control/textual/inline_object.h>

namespace zaf::textual {

AttachedInfo::AttachedInfo(std::shared_ptr<InlineObject> source) : EventInfo(std::move(source)) {

}


DetachedInfo::DetachedInfo(std::shared_ptr<InlineObject> source) : EventInfo(std::move(source)) {

}

}