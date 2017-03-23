#pragma once

#include <zaf/base/timer.h>
#include <zaf/control/internal/image_box/image_player.h>
#include <zaf/graphic/image/gif_metadata_querier.h>
#include <zaf/graphic/image/image_decoder.h>
#include <zaf/graphic/renderer/bitmap_renderer.h>

namespace zaf {
namespace internal {

class GifPlayer : public ImagePlayer {
public:
    GifPlayer(const ImageDecoder& image_decoder);

    const Bitmap GetBitmap(Renderer& renderer) override;
    void Reset() override;

private:
    bool Initialize(Renderer& renderer);
    bool InitializeComposedFrameRenderer(Renderer& renderer, const GifGlobalMetadataQuerier& metadata_querier);
    void InitializeBackgroundColor(const GifGlobalMetadataQuerier& metadata_querier);
    void ComposeNextFrame();
    void DisposeCurrentFrame();
    void ClearBackground();
    void OverlayNextFrame();
    void SaveFrame();
    void RestoreFrame();
    void StartTimer();
    void TimerTriggered();

    bool HasReachedLastLoop() const;

    bool HasReachedLastFrame() const {
        return next_frame_index_ == 0;
    }

private:
    ImageDecoder image_decoder_;
    
    BitmapRenderer composed_frame_renderer_;
    std::size_t frame_count_;
    Color background_color_;
    zaf::optional<std::size_t> total_loop_count_;
    std::shared_ptr<Timer> delay_timer_;

    std::size_t next_frame_index_;
    std::uint16_t current_frame_delay_;
    GifDisposal current_frame_disposal_;
    Rect current_frame_rect_;
    Bitmap saved_frame_bitmap_;
    std::size_t current_loop_count_;
};

}
}