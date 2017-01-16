#pragma once

#include <wincodec.h>
#include <memory>
#include <zaf/base/error.h>

namespace zaf {

class ImageDecoder {
public:
    class Frame {
    public:
        explicit Frame(IWICBitmapFrameDecode* handle) : handle_(handle) { 
        
        }

        ~Frame() {
            handle_->Release();
        }

        IWICBitmapFrameDecode* GetHandle() const {
            return handle_;
        }

        Frame(const Frame&) = delete;
        Frame& operator=(const Frame&) = delete;

    private:
        IWICBitmapFrameDecode* handle_;
    };

public:
    explicit ImageDecoder(IWICBitmapDecoder* handle) : handle_(handle) {

    }

    ~ImageDecoder() {
        handle_->Release();
    }

    std::size_t GetFrameCount(std::error_code& error_code) const;

    std::size_t GetFrameCount() const {
        std::error_code error_code;
        auto result = GetFrameCount(error_code);
        ZAF_CHECK_ERROR(error_code);
        return result;
    }

    const std::shared_ptr<Frame> GetFrame(std::size_t index, std::error_code& error_code) const;

    const std::shared_ptr<Frame> GetFrame(std::size_t index) const {
        std::error_code error_code;
        auto result = GetFrame(index, error_code);
        ZAF_CHECK_ERROR(error_code);
        return result;
    }

    IWICBitmapDecoder* GetHandle() const {
        return handle_;
    }

    ImageDecoder(const ImageDecoder&) = delete;
    ImageDecoder& operator=(const ImageDecoder&) = delete;

private:
    IWICBitmapDecoder* handle_;
};

}