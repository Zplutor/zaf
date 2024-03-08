#pragma once

#include <zaf/base/as.h>
#include <zaf/control/textual/inline_object.h>
#include <zaf/object/object_type.h>

namespace zaf::internal {

class InlineObjectWrapper {
public:
    InlineObjectWrapper() = default;
    explicit InlineObjectWrapper(std::shared_ptr<textual::InlineObject> object) :
        object_(std::move(object)) {

    }

    ~InlineObjectWrapper() {
        if (object_) {
            object_->SetHost(nullptr);
        }
    }

    InlineObjectWrapper(const InlineObjectWrapper& other) {
        CopyFromOther(other);
    }

    InlineObjectWrapper& operator=(const InlineObjectWrapper& other) {
        CopyFromOther(other);
        return *this;
    }

    InlineObjectWrapper(InlineObjectWrapper&& other) noexcept {
        MoveFromOther(std::move(other));
    }

    InlineObjectWrapper& operator=(InlineObjectWrapper&& other) noexcept {
        MoveFromOther(std::move(other));
        return *this;
    }

    const std::shared_ptr<textual::InlineObject>& Object() const {
        return object_;
    }

    std::shared_ptr<textual::InlineObject> Detach() {
        return std::move(object_);
    }

private:
    void CopyFromOther(const InlineObjectWrapper& other) {
        if (other.object_) {
            object_ = As<textual::InlineObject>(other.object_->GetType()->CreateInstance());
        }
        else {
            object_ = nullptr;
        }
    }

    void MoveFromOther(InlineObjectWrapper&& other) {
        object_ = std::move(other.object_);
    }

private:
    std::shared_ptr<textual::InlineObject> object_;
};

}