#pragma once

#include <zaf/base/non_copyable.h>

namespace zaf::internal {

class ListControlPartContext;

class ListControlPart : NonCopyableNonMovable {
public:
    explicit ListControlPart(const ListControlPartContext* context) : context_(context) {

    }

    virtual ~ListControlPart() = default;

    const ListControlPartContext& Context() const {
        return *context_;
    }

private:
    const ListControlPartContext* context_{};
};

}