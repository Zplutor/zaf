#pragma once

#include <functional>
#include <zaf/graphic/image/bitmap.h>

namespace zaf {

class Renderer;

namespace internal {

class ImagePlayer {
public:
    typedef std::function<void()> UpdateEvent;

    void SetUpdateEvent(const UpdateEvent& event) {
        update_event_ = event;
    }

    virtual const Bitmap GetBitmap(Renderer& renderer) = 0;

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