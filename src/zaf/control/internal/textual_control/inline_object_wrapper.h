#pragma once

#include <zaf/base/as.h>
#include <zaf/graphic/text/custom_text_inline_object.h>
#include <zaf/object/object_type.h>

namespace zaf::internal {

class InlineObjectWrapper {
public:
    InlineObjectWrapper() = default;
    explicit InlineObjectWrapper(std::shared_ptr<CustomTextInlineObject> object) :
        object_(std::move(object)) {

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

    const std::shared_ptr<CustomTextInlineObject>& Object() const {
        return object_;
    }

private:
    void CopyFromOther(const InlineObjectWrapper& other) {
        if (other.object_) {
            object_ = As<CustomTextInlineObject>(other.object_->GetType()->CreateInstance());
        }
        else {
            object_ = nullptr;
        }
    }

    void MoveFromOther(InlineObjectWrapper&& other) {
        object_ = std::move(other.object_);
    }

private:
    std::shared_ptr<CustomTextInlineObject> object_;
};

}