#pragma once

#include <functional>
#include <zaf/graphic/render_bitmap.h>

namespace zaf {

class Renderer;

namespace internal {

class ImagePlayer {
public:
    typedef std::function<void()> UpdateEvent;

    void SetUpdateEvent(const UpdateEvent& event) {
        update_event_ = event;
    }

    virtual const RenderBitmap GetBitmap(Renderer& renderer) = 0;

    virtual void Reset() { }

protected:
    void NotifyUpdate() {
        if (update_event_ != nullptr) {
            update_event_();
        }
    }

private:
    UpdateEvent update_event_;
};

}
}