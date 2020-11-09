#include <zaf/control/internal/image_box/gif_player.h>
#include <zaf/application.h>
#include <zaf/graphic/image/wic/imaging_factory.h>

namespace zaf {
namespace internal {

GifPlayer::GifPlayer(const wic::BitmapDecoder& image_decoder) : 
    image_decoder_(image_decoder),
    frame_count_(0),
    background_color_(Color::Transparent()),
    next_frame_index_(0),
    current_frame_delay_(0),
    current_frame_disposal_(GifDisposal::Unspecified),
    current_loop_count_(0) {

}


void GifPlayer::GetImageSize(Size& pixel_size, std::pair<float, float>& resolution) {

    try {

        auto first_frame = image_decoder_.GetFrame(0);
        pixel_size = first_frame.GetSize();

        auto origin_resolution = first_frame.GetResolution();
        resolution.first = static_cast<float>(origin_resolution.first);
        resolution.second = static_cast<float>(origin_resolution.second);
    }
    catch (const Error&) {

    }
}


RenderBitmap GifPlayer::GetRenderBitmap(Renderer& renderer) {
    
    try {
        if (Initialize(renderer)) {
            return composed_frame_renderer_.GetBitmap();
        }
        return {};
    }
    catch (const Error&) {
        return {};
    }
}


bool GifPlayer::Initialize(Renderer& renderer) {

    if (composed_frame_renderer_ != nullptr) {
        return true;
    }

    frame_count_ = image_decoder_.GetFrameCount();
    if (frame_count_ == 0) {
        return false;
    }

    auto global_metadata_querier = image_decoder_.GetMetadataQuerier();
    GifGlobalMetadataQuerier gif_global_metadata_querier(global_metadata_querier);

    InitializeComposedFrameRenderer(renderer, gif_global_metadata_querier);

    InitializeBackgroundColor(gif_global_metadata_querier);
    total_loop_count_ = gif_global_metadata_querier.GetLoopCount();

    next_frame_index_ = 0;
    current_frame_delay_ = 0;
    current_frame_disposal_ = GifDisposal::Unspecified; 
    ComposeNextFrame();
    return true;
}


void GifPlayer::InitializeComposedFrameRenderer(
    Renderer& renderer,
    const GifGlobalMetadataQuerier& metadata_querier) {

    auto width = metadata_querier.GetWidth();
    auto height = metadata_querier.GetHeight();

    CreateCompatibleRendererOptions options;
    options.DesiredSize(Size(width, height));
    composed_frame_renderer_ = renderer.CreateCompatibleRenderer(options);
}


void GifPlayer::InitializeBackgroundColor(const GifGlobalMetadataQuerier& metadata_querier) {

    if (! metadata_querier.HasGlobalColorTable()) {
        //There is no global color table, so there is no background color either.
        return;
    }

    //Get background color index.
    auto background_color_index = metadata_querier.GetBackgroundColorIndex();

    //Get palette from image decoder.
    auto palette = GetApplication().GetImagingFactory().CreatePalette();

    image_decoder_.CopyPalette(palette);

    //Get colors from palette.
    auto color_count = palette.GetColorCount();
    auto colors = std::make_unique<std::uint32_t[]>(color_count);
    palette.GetColors(color_count, colors.get(), color_count);

    //Check whether background color index is out of range.
    if (background_color_index >= color_count) {
        return;
    }

    //Get color.
    auto background_color_argb = colors[background_color_index];
    background_color_ = zaf::Color::FromARGB(background_color_argb);
}


void GifPlayer::ComposeNextFrame() {

    DisposeCurrentFrame();
    OverlayNextFrame();

    while ((current_frame_delay_ == 0) && ! HasReachedLastFrame()) {
        DisposeCurrentFrame();
        OverlayNextFrame();
    }

    if (! HasReachedLastLoop()) {
        StartTimer();
    }
}


void GifPlayer::DisposeCurrentFrame() {

    switch (current_frame_disposal_) {
    case GifDisposal::RestoreToBackgroundColor:
        ClearBackground();
        break;
    case GifDisposal::RestoreToPrevious:
        RestoreFrame();
        break;
    default:
        break;
    }
}


void GifPlayer::ClearBackground() {

    composed_frame_renderer_.BeginDraw();
    composed_frame_renderer_.PushAxisAlignedClipping(current_frame_rect_, AntialiasMode::PerPrimitive);
    composed_frame_renderer_.Clear(background_color_);
    composed_frame_renderer_.PopAxisAlignedClipping();
    composed_frame_renderer_.EndDraw();
}


void GifPlayer::OverlayNextFrame() {

    auto next_frame = image_decoder_.GetFrame(next_frame_index_);
    if (next_frame == nullptr) {
        return;
    }

    //Get metadata from frame.
    auto frame_metadata_querier = next_frame.GetMetadataQuerier();
    if (frame_metadata_querier == nullptr) {
        return;
    }

    GifFrameMetadataQuerier gif_frame_metadata_querier(frame_metadata_querier);
    current_frame_delay_ = gif_frame_metadata_querier.GetDelay();
    current_frame_disposal_ = gif_frame_metadata_querier.GetDisposal();

    if (current_frame_disposal_ == GifDisposal::RestoreToPrevious) {
        //Save previous frame before drawing current frame.
        SaveFrame();
    }

    std::uint16_t left = gif_frame_metadata_querier.GetLeft();
    std::uint16_t top = gif_frame_metadata_querier.GetTop();
    std::uint16_t width = gif_frame_metadata_querier.GetWidth();
    std::uint16_t height = gif_frame_metadata_querier.GetHeight();
    current_frame_rect_ = Rect(left, top, width, height);

    //Draw the frame into composed frame renderer.
    composed_frame_renderer_.BeginDraw();

    //Clear the background if a new loop is began.
    if (next_frame_index_ == 0) {
        composed_frame_renderer_.Clear(background_color_);
        current_loop_count_++;
    }
    
    auto bitmap = composed_frame_renderer_.CreateBitmap(next_frame);
    if (bitmap != nullptr) {

        composed_frame_renderer_.DrawBitmap(
            bitmap, 
            current_frame_rect_,
            1,
            InterpolationMode::Linear, 
            nullptr);
    }

    composed_frame_renderer_.EndDraw();

    //Advance to next frame.
    next_frame_index_ = (next_frame_index_ + 1) % frame_count_;
}


void GifPlayer::SaveFrame() {

    //Get current frame bitmap.
    auto current_bitmap = composed_frame_renderer_.GetBitmap();
    if (current_bitmap == nullptr) {
        return;
    }

    //Create the saved frame bitmap if it has not been created.
    if (saved_frame_bitmap_ == nullptr) {
        
        auto size = current_bitmap.GetPixelSize();
        auto dpi = current_bitmap.GetDpi();

        BitmapProperties bitmap_properties;
        bitmap_properties.dpi_x = dpi.first;
        bitmap_properties.dpi_y = dpi.second;
        bitmap_properties.pixel_properties = current_bitmap.GetPixelProperties();

        saved_frame_bitmap_ = composed_frame_renderer_.CreateBitmap(size, bitmap_properties);
        if (saved_frame_bitmap_ == nullptr) {
            return;
        }
    }

    //Copy bitmap to saved frame.
    saved_frame_bitmap_.CopyFromBitmap(current_bitmap);
}


void GifPlayer::RestoreFrame() {

    if (saved_frame_bitmap_ == nullptr) {
        return;
    }

    auto current_bitmap = composed_frame_renderer_.GetBitmap();
    if (current_bitmap == nullptr) {
        return;
    }

    current_bitmap.CopyFromBitmap(saved_frame_bitmap_);
}


void GifPlayer::StartTimer() {

    if (delay_timer_ == nullptr) {
        delay_timer_ = std::make_shared<Timer>(Timer::Mode::OneShot);
        Subscriptions() += delay_timer_->TriggerEvent().Subscribe(
            std::bind(&GifPlayer::TimerTriggered, this));
    }

    delay_timer_->SetInterval(std::chrono::milliseconds(current_frame_delay_ * 10));
    delay_timer_->Start();
}


void GifPlayer::TimerTriggered() {
    ComposeNextFrame();
    NotifyUpdate();
}


bool GifPlayer::HasReachedLastLoop() const {

    if (! HasReachedLastFrame()) {
        return false;
    }

    //Play once only if there is no loop count.
    if (! total_loop_count_.has_value()) {
        return true;
    }

    //Play infinite if loop count is zero.
    if (*total_loop_count_ == 0) {
        return false;
    }

    return current_loop_count_ < *total_loop_count_;
}


void GifPlayer::Reset() {

    composed_frame_renderer_.Reset();
    saved_frame_bitmap_.Reset();
    delay_timer_.reset();

    frame_count_ = 0;
    background_color_ = Color::Transparent();
    total_loop_count_.reset();
    next_frame_index_ = 0;
    current_frame_delay_ = 0;
    current_frame_disposal_ = GifDisposal::Unspecified;
    current_frame_rect_ = Rect();
    current_loop_count_ = 0;
}

}
}