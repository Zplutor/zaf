#pragma once

#include <zaf/base/direct2d.h>
#include <zaf/graphic/size.h>

namespace zaf {

class Bitmap {
public:
    explicit Bitmap(ID2D1Bitmap* handle) : handle_(handle) { 
    
    }

    ~Bitmap() {
        handle_->Release();
    }

    /**
     Get the size, in device-independent pixels (DIPs), of the bitmap.
     */
    const Size GetSize() const {
        return Size::FromD2D1SIZEF(handle_->GetSize());
    }

    /**
     Get the size, in device-dependent units (pixels), of the bitmap.
     */
    const Size GetPixelSize() const {
        return Size::FromD2D1SIZEU(handle_->GetPixelSize());
    }

    /**
     Get dots per inch (DPI) of the bitmap.

     @return
         The first in returned pair is horizontal DPI, and
         the second is vertical DPI.
     */
    const std::pair<float, float> GetDpi() const {
        std::pair<float, float> pair;
        handle_->GetDpi(&pair.first, &pair.second);
        return pair;
    }

    /**
     Get the underlying ID2D1Bitmap* handle.
     */
    ID2D1Bitmap* GetHandle() const {
        return handle_;
    }

private:
    ID2D1Bitmap* handle_;
};

}