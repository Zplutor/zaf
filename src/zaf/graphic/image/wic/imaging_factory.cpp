#include <zaf/graphic/image/wic/imaging_factory.h>
#include <zaf/graphic/image/wic/stream.h>

namespace zaf::wic {

BitmapDecoder ImagingFactory::CreateDecoderFromFile(
    const std::filesystem::path& file_path,
    const DecoderCreateOptions& options) {

    IWICBitmapDecoder* handle{};
    HRESULT result = GetHandle()->CreateDecoderFromFilename(
        file_path.c_str(),
        nullptr,
        GENERIC_READ,
        static_cast<WICDecodeOptions>(options.decode_options),
        &handle);

    ZAF_THROW_IF_COM_ERROR(result);
    return BitmapDecoder{ handle };
}


BitmapDecoder ImagingFactory::CreateDecoderFromStream(
    const Stream& stream,
    const DecoderCreateOptions& options) {

    IWICBitmapDecoder* handle{};
    HRESULT com_error = GetHandle()->CreateDecoderFromStream(
        stream.GetHandle(),
        nullptr,
        static_cast<WICDecodeOptions>(options.decode_options),
        &handle);

    ZAF_THROW_IF_COM_ERROR(com_error);
    return BitmapDecoder{ handle };
}


Bitmap ImagingFactory::CreateBitmap(const Size& size, const BitmapCreateOptions& options) {

    IWICBitmap* handle{};
    HRESULT com_error = GetHandle()->CreateBitmap(
        static_cast<UINT>(size.width),
        static_cast<UINT>(size.height),
        ToWICPixelFormatGUID(options.pixel_format),
        static_cast<WICBitmapCreateCacheOption>(options.cache_option),
        &handle);

    ZAF_THROW_IF_COM_ERROR(com_error);
    return Bitmap{ handle };
}

}