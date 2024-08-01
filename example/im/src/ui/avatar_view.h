#pragma once

#include <zaf/control/control.h>
#include <zaf/graphic/image/wic/bitmap_source.h>

class AvatarView : public zaf::Control {
public:
    void SetImageSource(const zaf::wic::BitmapSource& image_source) {
        image_source_ = image_source;
        NeedRepaint();
    }

protected:
    void Paint(zaf::Canvas& canvas, const zaf::Rect& dirty_rect) const override;

private:
    zaf::wic::BitmapSource image_source_;
};