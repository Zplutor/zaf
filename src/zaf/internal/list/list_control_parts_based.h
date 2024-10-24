#pragma once

#include <zaf/base/non_copyable.h>

namespace zaf::internal {

class ListControlPartsContext;

class ListControlPartsBased : NonCopyableNonMovable {
public:
    explicit ListControlPartsBased(const ListControlPartsContext* parts) : parts_(parts) {

    }

    virtual ~ListControlPartsBased() = default;

    const ListControlPartsContext& Parts() const {
        return *parts_;
    }

private:
    const ListControlPartsContext* parts_{};
};

}