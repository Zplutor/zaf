#pragma once

#include <zaf/base/flag_enum.h>
#include <zaf/graphic/image/wic/bitmap_source.h>

namespace zaf::wic {

class Palette;

class Bitmap : public BitmapSource {
public:
    class Lock : public COMObject<IWICBitmapLock> {
    public:
        using COMObject::COMObject;

        Size GetSize() const;

        std::uint32_t GetStride() const;

        void GetDataPointer(std::uint8_t*& data_pointer, std::size_t& data_size) const;
    };

    enum LockFlag {
        Read = WICBitmapLockRead,
        Write = WICBitmapLockWrite,
    };

public:
    Bitmap() = default;
    explicit Bitmap(COMPtr<IWICBitmap> ptr) : BitmapSource(ptr), inner_(std::move(ptr)) { }
    
    void SetResolution(double x, double y);

    void SetPalette(const Palette& palette);

    Lock GetLock(const Rect& rect, LockFlag flags);

    const COMPtr<IWICBitmap>& Inner() const noexcept {
        return inner_;
    }

private:
    COMPtr<IWICBitmap> inner_;
};


ZAF_ENABLE_FLAG_ENUM(Bitmap::LockFlag);

}