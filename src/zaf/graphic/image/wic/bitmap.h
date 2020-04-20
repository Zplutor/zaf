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

        Size GetSize(std::error_code& error_code) const;

        Size GetSize() const {
            std::error_code error_code;
            auto result = GetSize(error_code);
            ZAF_CHECK_ERROR(error_code);
            return result;
        }

        std::uint32_t GetStride(std::error_code& error_code) const;

        std::uint32_t GetStride() const {
            std::error_code error_code;
            auto result = GetStride(error_code);
            ZAF_CHECK_ERROR(error_code);
            return result;
        }

        void GetDataPointer(
            std::uint8_t*& data_pointer, 
            std::size_t& data_size, 
            std::error_code& error_code) const;

        void GetDataPointer(std::uint8_t*& data_pointer, std::size_t& data_size) const {
            std::error_code error_code;
            GetDataPointer(data_pointer, data_size, error_code);
            ZAF_CHECK_ERROR(error_code);
        }
    };

    enum LockFlag {
        Read = WICBitmapLockRead,
        Write = WICBitmapLockWrite,
    };

public:
    Bitmap() { }
    explicit Bitmap(IWICBitmap* handle) : BitmapSource(handle) { }
    
    void SetResolution(double x, double y, std::error_code& error_code);

    void SetResolution(double x, double y) {
        std::error_code error_code;
        SetResolution(x, y, error_code);
        ZAF_CHECK_ERROR(error_code);
    }

    void SetPalette(const Palette& palette, std::error_code& error_code);

    void SetPalette(const Palette& palette) {
        std::error_code error_code;
        SetPalette(palette, error_code);
        ZAF_CHECK_ERROR(error_code);
    }

    Lock GetLock(const Rect& rect, LockFlag flags, std::error_code& error_code);

    Lock GetLock(const Rect& rect, LockFlag flags) {
        std::error_code error_code;
        auto result = GetLock(rect, flags, error_code);
        ZAF_CHECK_ERROR(error_code);
        return result;
    }

    IWICBitmap* GetHandle() const {
        return static_cast<IWICBitmap*>(__super::GetHandle());
    }
};


ZAF_ENABLE_FLAG_ENUM(Bitmap::LockFlag);

}