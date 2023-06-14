#pragma once

#include <wincodec.h>
#include <filesystem>
#include <zaf/base/com_object.h>
#include <zaf/base/error/com_error.h>
#include <zaf/graphic/image/wic/bitmap.h>
#include <zaf/graphic/image/wic/bitmap_decoder.h>
#include <zaf/graphic/image/wic/bitmap_scaler.h>
#include <zaf/graphic/image/wic/palette.h>
#include <zaf/graphic/image/wic/pixel_format.h>
#include <zaf/graphic/image/wic/stream.h>

namespace zaf {

class Application;

namespace wic {

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


class ImagingFactory : public COMObject<IWICImagingFactory> {
public:
    static ImagingFactory& Instance();

public:
    wic::Stream CreateStream() {
        COMPtr<IWICStream> ptr;
        HRESULT com_error = Inner()->CreateStream(ptr.Reset());
        ZAF_THROW_IF_COM_ERROR(com_error);
        return wic::Stream{ ptr };
    }


    BitmapDecoder CreateDecoderFromFile(
        const std::filesystem::path& file_path,
        const DecoderCreateOptions& options);

    BitmapDecoder CreateDecoderFromFile(const std::filesystem::path& file_path) {
        return CreateDecoderFromFile(file_path, DecoderCreateOptions{});
    }


    BitmapDecoder CreateDecoderFromStream(
        const Stream& image_stream,
        const DecoderCreateOptions& options);

    BitmapDecoder CreateDecoderFromStream(const Stream& image_stream) {
        return CreateDecoderFromStream(image_stream, DecoderCreateOptions{});
    }


    Bitmap CreateBitmap(const Size& size, const BitmapCreateOptions& options);

    Bitmap CreateBitmap(const Size& size) {
        return CreateBitmap(size, BitmapCreateOptions{});
    }


    Bitmap CreateBitmapFromHBITMAP(
        HBITMAP bitmap,
        const BitmapCreateFromHBITMAPOptions& options) {

        COMPtr<IWICBitmap> ptr;
        HRESULT com_error = Inner()->CreateBitmapFromHBITMAP(
            bitmap,
            options.palette,
            static_cast<WICBitmapAlphaChannelOption>(options.alpha_channel_option),
            ptr.Reset());

        ZAF_THROW_IF_COM_ERROR(com_error);
        return Bitmap{ ptr };
    }

    Bitmap CreateBitmapFromHBITMAP(HBITMAP bitmap) {
        return CreateBitmapFromHBITMAP(bitmap, BitmapCreateFromHBITMAPOptions{});
    }

    BitmapScaler CreateBitmapScaler() {
        COMPtr<IWICBitmapScaler> ptr;
        HRESULT com_error = Inner()->CreateBitmapScaler(ptr.Reset());
        ZAF_THROW_IF_COM_ERROR(com_error);
        return wic::BitmapScaler{ ptr };
    }


    Palette CreatePalette() {
        COMPtr<IWICPalette> ptr;
        HRESULT com_error = Inner()->CreatePalette(ptr.Reset());
        ZAF_THROW_IF_COM_ERROR(com_error);
        return Palette{ ptr };
    }

private:
    friend class zaf::Application;

    using COMObject::COMObject;
};

}
}