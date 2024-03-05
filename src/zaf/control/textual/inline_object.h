#pragma once

#include <zaf/base/non_copyable.h>
#include <zaf/control/textual/inline_object_events.h>
#include <zaf/object/object.h>
#include <zaf/graphic/text/text_inline_object_metrics.h>

namespace zaf {

class Canvas;
class TextualControl;

namespace internal {
class TextInlineObjectBridge;
}

namespace textual {

class InlineObject : public Object, NonCopyableNonMovable {
public:
    ZAF_DECLARE_TYPE;

public:
    InlineObject() = default;

    std::shared_ptr<TextualControl> Host() const {
        return host_.lock();
    }

    virtual TextInlineObjectMetrics GetMetrics() const;

protected:
    virtual void Paint(Canvas& canvas) const;
    virtual void OnMouseCursorChanging(const textual::MouseCursorChangingInfo& event_info);
    virtual void OnMouseDown();
    virtual void OnMouseUp();

private:
    friend class TextualControl;
    friend class internal::TextInlineObjectBridge;

    void SetHost(std::shared_ptr<TextualControl> host) {
        host_ = std::move(host);
    }

private:
    std::weak_ptr<TextualControl> host_;
};

}
}