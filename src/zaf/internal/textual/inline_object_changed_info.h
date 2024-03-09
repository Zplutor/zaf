#pragma once

#include <zaf/control/textual/inline_object.h>

namespace zaf::internal {

class InlineObjectChangedInfo {
public:
    InlineObjectChangedInfo(
        std::vector<std::shared_ptr<textual::InlineObject>> attached_objects,
        std::vector<std::shared_ptr<textual::InlineObject>> detached_objects)
        :
        attached_objects_(std::move(attached_objects)),
        detached_objects_(std::move(detached_objects)) {

    }

    const std::vector<std::shared_ptr<textual::InlineObject>>& AttachedObjects() const {
        return attached_objects_;
    }

    const std::vector<std::shared_ptr<textual::InlineObject>>& DetachedObjects() const {
        return detached_objects_;
    }

private:
    std::vector<std::shared_ptr<textual::InlineObject>> attached_objects_;
    std::vector<std::shared_ptr<textual::InlineObject>> detached_objects_;
};

}