#include <zaf/control/internal/image_box/gif_player.h>
#include <zaf/application.h>
#include <zaf/graphic/resource_factory.h>

namespace zaf {
namespace internal {

GifPlayer::GifPlayer(const ImageDecoder& image_decoder) : 
    image_decoder_(image_decoder),
    frame_count_(0),
    background_color_(Color::Transparent),
    next_frame_index_(0),
    current_frame_delay_(0),
    current_frame_disposal_(GifDisposal::Unspecified) {

}


const Bitmap GifPlayer::GetBitmap(Renderer& renderer) {
    
    if (Initialize(renderer)) {
        return composed_frame_renderer_.GetBitmap();   
    }
    else {
        return Bitmap();
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
    if (global_metadata_querier == nullptr) {
        return false;
    }

    GifGlobalMetadataQuerier gif_global_metadata_querier(global_metadata_querier);
    
    if (! InitializeComposedFrameRenderer(renderer, gif_global_metadata_querier)) {
        return false;
    }

    InitializeBackgroundColor(gif_global_metadata_querier);

    next_frame_index_ = 0;
    current_frame_delay_ = 0;
    current_frame_disposal_ = GifDisposal::Unspecified; 
    ComposeNextFrame();
    return true;
}


bool GifPlayer::InitializeComposedFrameRenderer(Renderer& renderer, const GifGlobalMetadataQuerier& metadata_querier) {

    auto width = metadata_querier.GetWidth();
    auto height = metadata_querier.GetHeight();

    CreateCompatibleRendererOptions options;
    options.DesiredSize(Size(width, height));
    composed_frame_renderer_ = renderer.CreateCompatibleRenderer(options);
    return composed_frame_renderer_ != nullptr;
}


void GifPlayer::InitializeBackgroundColor(const GifGlobalMetadataQuerier& metadata_querier) {

    if (! metadata_querier.HasGlobalColorTable()) {
        //There is no global color table, so there is no background color either.
        return;
    }

    //Get background color index.
    std::error_code error_code;
    auto background_color_index = metadata_querier.GetBackgroundColorIndex(error_code);
    if (error_code) {
        return;
    }

    //Get palette from image decoder.
    auto palette = GetResourceFactory()->CreateImagePalette();
    if (palette == nullptr) {
        return;
    }

    image_decoder_.CopyPalette(palette, error_code);
    if (error_code) {
        return;
    }

    //Get colors from palette.
    auto color_count = palette.GetColorCount();
    auto colors = std::make_unique<std::uint32_t[]>(color_count);
    palette.GetColors(color_count, colors.get(), color_count, error_code);
    if (error_code) {
        return;
    }

    //Check whether background color index is out of range.
    if (background_color_index >= color_count) {
        return;
    }

    //Get color.
    auto background_color_argb = colors[background_color_index];
    background_color_ = zaf::Color::FromRGB(background_color_argb);
}


void GifPlayer::ComposeNextFrame() {

    DisposeCurrentFrame();
    OverlayNextFrame();

    while ((current_frame_delay_ == 0) && ! HasReachedLastFrame()) {
        DisposeCurrentFrame();
        OverlayNextFrame();
    }

    StartTimer();
}


void GifPlayer::DisposeCurrentFrame() {

    switch (current_frame_disposal_) {
    case GifDisposal::RestoreToBackgroundColor:
        composed_frame_renderer_.Clear(background_color_);
        break;
    case GifDisposal::RestoreToPrevious:
        break;
    default:
        break;
    }
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

    std::uint16_t left = gif_frame_metadata_querier.GetLeft();
    std::uint16_t top = gif_frame_metadata_querier.GetTop();
    std::uint16_t width = gif_frame_metadata_querier.GetWidth();
    std::uint16_t height = gif_frame_metadata_querier.GetHeight();

    //Draw the frame into composed frame renderer.
    composed_frame_renderer_.BeginDraw();
    
    auto bitmap = composed_frame_renderer_.CreateBitmap(next_frame);
    if (bitmap != nullptr) {

        composed_frame_renderer_.DrawBitmap(
            bitmap, 
            Rect(left, top, width, height),
            1,
            InterpolationMode::Linear, 
            nullptr);
    }

    composed_frame_renderer_.EndDraw();

    //Advance to next frame.
    next_frame_index_ = (next_frame_index_ + 1) % frame_count_;
}


void GifPlayer::StartTimer() {

    if (delay_timer_ == nullptr) {
        delay_timer_ = std::make_unique<Timer>(Timer::Mode::OneShot);
        delay_timer_->GetTriggerEvent().AddListener(std::bind(&GifPlayer::TimerTriggered, this));
    }

    auto interval_milliseconds = current_frame_delay_ * 10;
    if (interval_milliseconds < 90) {
        interval_milliseconds = 90;
    }

    delay_timer_->SetInterval(std::chrono::milliseconds(interval_milliseconds));
    delay_timer_->Start();
}


void GifPlayer::TimerTriggered() {
    ComposeNextFrame();
    NotifyUpdate();
}


void GifPlayer::Reset() {

    composed_frame_renderer_.Reset();
    delay_timer_.reset();

    frame_count_ = 0;
    background_color_ = Color::Transparent;
    next_frame_index_ = 0;
    current_frame_delay_ = 0;
    current_frame_disposal_ = GifDisposal::Unspecified;
}

}
}