#include <zaf/graphic/image/wic/imaging_factory.h>
#include <zaf/application.h>
#include <zaf/graphic/image/wic/stream.h>

namespace zaf::wic {

ImagingFactory& ImagingFactory::Instance() {
    return Application::Instance().GetImagingFactory();
}


BitmapDecoder ImagingFactory::CreateDecoderFromFile(
    const std::filesystem::path& file_path,
    const DecoderCreateOptions& options) {

    COMPtr<IWICBitmapDecoder> ptr;
    HRESULT result = Inner()->CreateDecoderFromFilename(
        file_path.c_str(),
        nullptr,
        GENERIC_READ,
        static_cast<WICDecodeOptions>(options.decode_options),
        ptr.Reset());

    ZAF_THROW_IF_COM_ERROR(result);
    return BitmapDecoder{ ptr };
}


BitmapDecoder ImagingFactory::CreateDecoderFromStream(
    const Stream& stream,
    const DecoderCreateOptions& options) {

    COMPtr<IWICBitmapDecoder> ptr;
    HRESULT com_error = Inner()->CreateDecoderFromStream(
        stream.Inner().Inner(),
        nullptr,
        static_cast<WICDecodeOptions>(options.decode_options),
        ptr.Reset());

    ZAF_THROW_IF_COM_ERROR(com_error);
    return BitmapDecoder{ ptr };
}


Bitmap ImagingFactory::CreateBitmap(const Size& size, const BitmapCreateOptions& options) {

    COMPtr<IWICBitmap> ptr;
    HRESULT com_error = Inner()->CreateBitmap(
        static_cast<UINT>(size.width),
        static_cast<UINT>(size.height),
        ToWICPixelFormatGUID(options.pixel_format),
        static_cast<WICBitmapCreateCacheOption>(options.cache_option),
        ptr.Reset());

    ZAF_THROW_IF_COM_ERROR(com_error);
    return Bitmap{ ptr };
}

}