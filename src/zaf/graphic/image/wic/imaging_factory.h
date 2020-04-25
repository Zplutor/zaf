#pragma once

#include <wincodec.h>
#include <filesystem>
#include <zaf/base/com_object.h>
#include <zaf/graphic/image/wic/bitmap.h>
#include <zaf/graphic/image/wic/bitmap_decoder.h>
#include <zaf/graphic/image/wic/bitmap_scaler.h>
#include <zaf/graphic/image/wic/palette.h>
#include <zaf/graphic/image/wic/pixel_format.h>
#include <zaf/graphic/image/wic/stream.h>

namespace zaf::wic {

class Stream;

enum class DecodeOptions {
    MetadataCacheOnDemand = WICDecodeMetadataCacheOnDemand,
    MetadataCacheOnLoad = WICDecodeMetadataCacheOnLoad,
};

class DecoderCreateOptions {
public:
    DecodeOptions decode_options{ DecodeOptions::MetadataCacheOnDemand };
};


enum class BitmapCacheOption {
    NoCache = WICBitmapNoCache,
    CacheOnDemand = WICBitmapCacheOnDemand,
    CacheOnLoad = WICBitmapCacheOnLoad,
};

class BitmapCreateOptions {
public:
    //TODO: Which format should be the default value?
    PixelFormat pixel_format{ PixelFormat::BGR32 }; 
    BitmapCacheOption cache_option{ BitmapCacheOption::CacheOnDemand };
};


enum class BitmapAlphaChannelOption {
    UseAlpha = WICBitmapUseAlpha,
    UsePremultipliedAlpha = WICBitmapUsePremultipliedAlpha,
    IgnoreAlpha = WICBitmapIgnoreAlpha,
};

class BitmapCreateFromHBITMAPOptions {
public:
    HPALETTE palette{};
    BitmapAlphaChannelOption alpha_channel_option{ BitmapAlphaChannelOption::UseAlpha };
};


class ImagingFactory : public ComObject<IWICImagingFactory> {
public:
    ImagingFactory() = default;
    explicit ImagingFactory(IWICImagingFactory* handle) : ComObject(handle) { }

    IWICImagingFactory* GetHandle() const {
        return static_cast<IWICImagingFactory*>(__super::GetHandle());
    }


    wic::Stream CreateStream(std::error_code& error_code) {
        IWICStream* handle{};
        HRESULT com_error = GetHandle()->CreateStream(&handle);
        error_code = MakeComErrorCode(com_error);
        return wic::Stream{ handle };
    }

    wic::Stream CreateStream() {
        std::error_code error_code;
        auto result = CreateStream(error_code);
        ZAF_CHECK_ERROR(error_code);
        return result;
    }


    BitmapDecoder CreateDecoderFromFile(
        const std::filesystem::path& file_path, 
        const DecoderCreateOptions& options,
        std::error_code& error_code);

    BitmapDecoder CreateDecoderFromFile(
        const std::filesystem::path& file_path,
        const DecoderCreateOptions& options) {

        std::error_code error_code;
        auto result = CreateDecoderFromFile(file_path, options, error_code);
        ZAF_CHECK_ERROR(error_code);
        return result;
    }

    BitmapDecoder CreateDecoderFromFile(const std::filesystem::path& file_path) {
        return CreateDecoderFromFile(file_path, DecoderCreateOptions{});
    }

    BitmapDecoder CreateDecoderFromFile(
        const std::filesystem::path& file_path,
        std::error_code& error_code) {

        return CreateDecoderFromFile(file_path, DecoderCreateOptions{}, error_code);
    }


    BitmapDecoder CreateDecoderFromStream(
        const Stream& image_stream,
        const DecoderCreateOptions& options,
        std::error_code& error_code);

    BitmapDecoder CreateDecoderFromStream(
        const Stream& image_stream, 
        const DecoderCreateOptions& options) {

        std::error_code error_code;
        auto result = CreateDecoderFromStream(image_stream, options, error_code);
        ZAF_CHECK_ERROR(error_code);
        return result;
    }

    BitmapDecoder CreateDecoderFromStream(const Stream& image_stream, std::error_code& error_code) {
        return CreateDecoderFromStream(image_stream, DecoderCreateOptions{}, error_code);
    }

    BitmapDecoder CreateDecoderFromStream(const wic::Stream& image_stream) {
        return CreateDecoderFromStream(image_stream, DecoderCreateOptions{});
    }


    Bitmap CreateBitmap(
        const Size& size,
        const BitmapCreateOptions& options,
        std::error_code& error_code);

    Bitmap CreateBitmap(const Size& size, const BitmapCreateOptions& options) {
        std::error_code error_code;
        auto result = CreateBitmap(size, options, error_code);
        ZAF_CHECK_ERROR(error_code);
        return result;
    }

    Bitmap CreateBitmap(const Size& size, std::error_code& error_code) {
        return CreateBitmap(size, BitmapCreateOptions{}, error_code);
    }

    Bitmap CreateBitmap(const Size& size) {
        return CreateBitmap(size, BitmapCreateOptions{});
    }


    Bitmap CreateBitmapFromHBITMAP(
        HBITMAP bitmap,
        const BitmapCreateFromHBITMAPOptions& options,
        std::error_code& error_code) {

        IWICBitmap* handle{};
        HRESULT com_error = GetHandle()->CreateBitmapFromHBITMAP(
            bitmap,
            options.palette,
            static_cast<WICBitmapAlphaChannelOption>(options.alpha_channel_option),
            &handle);

        error_code = MakeComErrorCode(com_error);
        return Bitmap{ handle };
    }

    Bitmap CreateBitmapFromHBITMAP(HBITMAP bitmap, const BitmapCreateFromHBITMAPOptions& options) {
        std::error_code error_code;
        auto result = CreateBitmapFromHBITMAP(bitmap, options, error_code);
        ZAF_CHECK_ERROR(error_code);
        return result;
    }

    Bitmap CreateBitmapFromHBITMAP(HBITMAP bitmap, std::error_code& error_code) {
        return CreateBitmapFromHBITMAP(bitmap, BitmapCreateFromHBITMAPOptions{}, error_code);
    }

    Bitmap CreateBitmapFromHBITMAP(HBITMAP bitmap) {
        return CreateBitmapFromHBITMAP(bitmap, BitmapCreateFromHBITMAPOptions{});
    }


    BitmapScaler CreateBitmapScaler(std::error_code& error_code) {
        IWICBitmapScaler* handle{};
        HRESULT com_error = GetHandle()->CreateBitmapScaler(&handle);
        error_code = MakeComErrorCode(com_error);
        return wic::BitmapScaler{ handle };
    }

    BitmapScaler CreateBitmapScaler() {
        std::error_code error_code;
        auto result = CreateBitmapScaler(error_code);
        ZAF_CHECK_ERROR(error_code);
        return result;
    }


    Palette CreatePalette(std::error_code& error_code) {
        IWICPalette* handle{};
        HRESULT com_error = GetHandle()->CreatePalette(&handle);
        error_code = MakeComErrorCode(com_error);
        return Palette{ handle };
    }

    Palette CreatePalette() {
        std::error_code error_code;
        auto result = CreatePalette(error_code);
        ZAF_CHECK_ERROR(error_code);
        return result;
    }
};

}