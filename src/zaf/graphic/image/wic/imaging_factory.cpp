#include <zaf/graphic/image/wic/imaging_factory.h>
#include <zaf/graphic/image/wic/stream.h>

namespace zaf::wic {

BitmapDecoder ImagingFactory::CreateDecoderFromFile(
    const std::filesystem::path& file_path,
    const DecoderCreateOptions& options,
    std::error_code& error_code) {

    IWICBitmapDecoder* handle{};
    HRESULT result = GetHandle()->CreateDecoderFromFilename(
        file_path.c_str(),
        nullptr,
        GENERIC_READ,
        static_cast<WICDecodeOptions>(options.decode_options),
        &handle);

    error_code = MakeComErrorCode(result);
    return BitmapDecoder{ handle };
}


BitmapDecoder ImagingFactory::CreateDecoderFromStream(
    const Stream& stream,
    const DecoderCreateOptions& options,
    std::error_code& error_code) {

    IWICBitmapDecoder* handle{};
    HRESULT com_error = GetHandle()->CreateDecoderFromStream(
        stream.GetHandle(),
        nullptr,
        static_cast<WICDecodeOptions>(options.decode_options),
        &handle);

    error_code = MakeComErrorCode(com_error);
    return BitmapDecoder{ handle };
}


Bitmap ImagingFactory::CreateBitmap(
    const Size& size,
    const BitmapCreateOptions& options,
    std::error_code& error_code) {

    IWICBitmap* handle{};
    HRESULT com_error = GetHandle()->CreateBitmap(
        static_cast<UINT>(size.width),
        static_cast<UINT>(size.height),
        ToWICPixelFormatGUID(options.pixel_format),
        static_cast<WICBitmapCreateCacheOption>(options.cache_option),
        &handle);

    error_code = MakeComErrorCode(com_error);
    return Bitmap{ handle };
}

}