#pragma once

#include <optional>
#include <zaf/base/event/event.h>
#include <zaf/base/non_copyable.h>
#include <zaf/base/range.h>
#include <zaf/control/textual/inline_object_events.h>
#include <zaf/graphic/size.h>
#include <zaf/graphic/text/text_inline_object_metrics.h>
#include <zaf/object/object.h>

namespace zaf {

class Canvas;
class TextualControl;

namespace internal {
class TextInlineObjectBridge;
}

namespace textual {

class InlineObjectCollection;

class InlineObject : 
    public Object, 
    public std::enable_shared_from_this<InlineObject>,
    NonCopyableNonMovable {

public:
    ZAF_DECLARE_TYPE;

public:
    InlineObject() = default;

    std::shared_ptr<TextualControl> Host() const noexcept {
        return host_.lock();
    }

    Observable<AttachedInfo> AttachedEvent() const {
        return attached_event_.GetObservable();
    }

    Observable<DetachedInfo> DetachedEvent() const {
        return detached_event_.GetObservable();
    }

    zaf::Size Size() const;
    virtual TextInlineObjectMetrics GetMetrics() const;

    /**
    Gets the range to which the inline object is attached in the host.

    @return
        A range to which the inline object is attached in the host. If the inline object is not 
        attached to any host, std::nullopt is returned.
    */
    std::optional<Range> RangeInHost() const noexcept;

protected:
    virtual void OnAttached(const AttachedInfo& event_info);
    virtual void OnDetached(const DetachedInfo& event_info);

    virtual void Paint(Canvas& canvas) const;

private:
    friend class InlineObjectCollection;
    friend class TextualControl;
    friend class internal::TextInlineObjectBridge;

    void SetHost(std::shared_ptr<TextualControl> host);
    void Detach();

private:
    //Accessed by InlineObjectCollection.
    std::optional<Range> attached_range_;

    std::weak_ptr<TextualControl> host_;
    bool has_set_host_{};

    Event<AttachedInfo> attached_event_;
    Event<DetachedInfo> detached_event_;
};

}
}