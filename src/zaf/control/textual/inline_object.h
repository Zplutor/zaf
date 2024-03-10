#pragma once

#include <zaf/base/event/event.h>
#include <zaf/base/non_copyable.h>
#include <zaf/control/textual/inline_object_events.h>
#include <zaf/object/object.h>
#include <zaf/graphic/text/text_inline_object_metrics.h>

namespace zaf {

class Canvas;
class TextualControl;

namespace internal {
class InlineObjectWrapper;
class TextInlineObjectBridge;
}

namespace textual {

class InlineObject : 
    public Object, 
    public std::enable_shared_from_this<InlineObject>,
    NonCopyableNonMovable {

public:
    ZAF_DECLARE_TYPE;

public:
    InlineObject() = default;

    Observable<AttachedInfo> AttachedEvent() const {
        return attached_event_.GetObservable();
    }

    Observable<DetachedInfo> DetachedEvent() const {
        return detached_event_.GetObservable();
    }

    std::shared_ptr<TextualControl> Host() const {
        return host_.lock();
    }

    virtual TextInlineObjectMetrics GetMetrics() const;

protected:
    virtual void OnAttached(const AttachedInfo& event_info);
    virtual void OnDetached(const DetachedInfo& event_info);

    virtual void Paint(Canvas& canvas) const;

private:
    friend class TextualControl;
    friend class internal::InlineObjectWrapper;
    friend class internal::TextInlineObjectBridge;

    void SetHost(std::shared_ptr<TextualControl> host);

private:
    std::weak_ptr<TextualControl> host_;

    Event<AttachedInfo> attached_event_;
    Event<DetachedInfo> detached_event_;
};

}
}