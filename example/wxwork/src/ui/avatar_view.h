#pragma once

#include <zaf/control/control.h>
#include <zaf/graphic/image/image_source.h>

class AvatarView : public zaf::Control {
public:
    void SetImageSource(const zaf::ImageSource& image_source) {
        image_source_ = image_source;
        NeedRepaint();
    }

protected:
    void Paint(zaf::Canvas& canvas, const zaf::Rect& dirty_rect) override;

private:
    zaf::ImageSource image_source_;
};