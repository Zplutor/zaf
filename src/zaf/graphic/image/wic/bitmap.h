#pragma once

#include <zaf/base/flag_enum.h>
#include <zaf/graphic/image/wic/bitmap_source.h>

namespace zaf::wic {

class Palette;

class Bitmap : public BitmapSource {
public:
    class Lock : public ComObject<IWICBitmapLock> {
    public:
        Lock() { }
        explicit Lock(IWICBitmapLock* handle) : ComObject(handle) { }

        Size GetSize() const;

        std::uint32_t GetStride() const;

        void GetDataPointer(std::uint8_t*& data_pointer, std::size_t& data_size) const;
    };

    enum LockFlag {
        Read = WICBitmapLockRead,
        Write = WICBitmapLockWrite,
    };

public:
    Bitmap() { }
    explicit Bitmap(IWICBitmap* handle) : BitmapSource(handle) { }
    
    void SetResolution(double x, double y);

    void SetPalette(const Palette& palette);

    Lock GetLock(const Rect& rect, LockFlag flags);

    IWICBitmap* GetHandle() const {
        return static_cast<IWICBitmap*>(__super::GetHandle());
    }
};


ZAF_ENABLE_FLAG_ENUM(Bitmap::LockFlag);

}