#pragma once

#include <optional>
#include <zaf/base/timer.h>
#include <zaf/control/internal/image_box/image_player.h>
#include <zaf/graphic/gif_metadata_querier.h>
#include <zaf/graphic/wic/bitmap_decoder.h>
#include <zaf/graphic/d2d/bitmap_renderer.h>
#include <zaf/rx/subscription_host.h>

namespace zaf {
namespace internal {

class GifPlayer : public ImagePlayer, public SubscriptionHost {
public:
    GifPlayer(const wic::BitmapDecoder& image_decoder);

    void GetImageSize(Size& pixel_size, std::pair<float, float>& resolution) override;
    d2d::RenderBitmap GetRenderBitmap(d2d::Renderer& renderer) override;
    void Reset() override;

private:
    bool Initialize(d2d::Renderer& renderer);
    void InitializeComposedFrameRenderer(
        d2d::Renderer& renderer, 
        const GifGlobalMetadataQuerier& metadata_querier);
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
    wic::BitmapDecoder image_decoder_;
    
    d2d::BitmapRenderer composed_frame_renderer_;
    std::size_t frame_count_;
    Color background_color_;
    std::optional<std::size_t> total_loop_count_;
    std::shared_ptr<Timer> delay_timer_;

    std::size_t next_frame_index_;
    std::uint16_t current_frame_delay_;
    GifDisposal current_frame_disposal_;
    Rect current_frame_rect_;
    d2d::RenderBitmap saved_frame_bitmap_;
    std::size_t current_loop_count_;
};

}
}