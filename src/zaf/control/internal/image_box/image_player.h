#pragma once

#include <functional>
#include <zaf/graphic/d2d/render_bitmap.h>

namespace zaf {

namespace d2d {
class Renderer;
}

namespace internal {

class ImagePlayer {
public:
    typedef std::function<void()> UpdateEvent;

    void SetUpdateEvent(const UpdateEvent& event) {
        update_event_ = event;
    }

    virtual void GetImageSize(Size& pixel_size, std::pair<float, float>& resolution) = 0;

    virtual d2d::RenderBitmap GetRenderBitmap(d2d::Renderer& renderer) = 0;

    virtual void ChangeDPI(float dpi) { }

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