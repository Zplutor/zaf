#pragma once

#include <zaf/control/textual/inline_object.h>

namespace zaf::internal {

class InlineObjectAttachedInfo {
public:
    std::vector<std::shared_ptr<textual::InlineObject>> attached_objects;
};

}