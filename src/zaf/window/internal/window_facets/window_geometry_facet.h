#pragma once

#include <zaf/base/none.h>
#include <zaf/base/non_copyable.h>
#include <zaf/graphic/frame.h>
#include <zaf/graphic/rect.h>
#include <zaf/rx/single.h>

namespace zaf {
class Message;
class Screen;
class Window;
}

namespace zaf::internal {

class WindowBasicStyle;
class WindowExtendedStyle;
class WindowNotCreatedStateData;

class WindowGeometryFacet : NonCopyableNonMovable {
public:
    static std::shared_ptr<zaf::Screen> ResolveInitialScreen(
        const internal::WindowNotCreatedStateData& state_data,
        const std::shared_ptr<Window>& owner) noexcept;

    static zaf::Rect ResolveInitialRect(
        const zaf::Screen& screen,
        const std::shared_ptr<Window>& owner,
        const internal::WindowNotCreatedStateData& state_data) noexcept;

public:
    explicit WindowGeometryFacet(Window& window) noexcept;

    std::shared_ptr<zaf::Screen> Screen() const noexcept;
    void SetScreen(std::shared_ptr<zaf::Screen> screen);
    float DPI() const noexcept;

    zaf::Rect Rect() const noexcept;
    void SetRect(const zaf::Rect& rect);
    Point Position() const noexcept;
    void SetPosition(const zaf::Point& position);
    zaf::Size Size() const noexcept;
    void SetSize(const zaf::Size& size);
    float Width() const noexcept;
    void SetWidth(float width);
    float Height() const noexcept;
    void SetHeight(float height);

    float MinWidth() const noexcept;
    void SetMinWidth(float min_width);
    float MaxWidth() const noexcept;
    void SetMaxWidth(float max_width);
    float MinHeight() const noexcept;
    void SetMinHeight(float min_height);
    float MaxHeight() const noexcept;
    void SetMaxHeight(float max_height);
    zaf::Size MinSize() const noexcept;
    void SetMinSize(const zaf::Size& size);
    zaf::Size MaxSize() const noexcept;
    void SetMaxSize(const zaf::Size& size);

    zaf::Rect ContentRect() const noexcept;
    zaf::Size ContentSize() const noexcept;
    void SetContentSize(const zaf::Size& size);
    float ContentWidth() const noexcept;
    void SetContentWidth(float width);
    float ContentHeight() const noexcept;
    void SetContentHeight(float height);

    void HandleWMSIZE(const Message& message);

    bool IsSizingOrMoving() const noexcept;
    rx::Single<None> WhenNotSizingOrMoving() const;
    void HandleWMENTERSIZEMOVE() noexcept;
    void HandleWMEXITSIZEMOVE();

    Point TransformToScreen(const Point& position) const noexcept;
    Point TransformFromScreen(const Point& position) const noexcept;
    zaf::Rect TransformToScreen(const zaf::Rect& rect) const noexcept;
    zaf::Rect TransformFromScreen(const zaf::Rect& rect) const noexcept;

private:
    zaf::Size ClampSize(const zaf::Size& size) const noexcept;
    void RaiseSizeChangedEvent();

    static Frame GetWindowFrame(
        float dpi,
        const internal::WindowBasicStyle& basic_style,
        const internal::WindowExtendedStyle& extend_style) noexcept;

    static zaf::Size AdjustContentSizeToWindowSize(
        const zaf::Size& content_size,
        float dpi,
        const internal::WindowBasicStyle& basic_style,
        const internal::WindowExtendedStyle& extend_style) noexcept;

    static zaf::Rect AdjustContentRectToWindowRect(
        const zaf::Rect& content_rect,
        float dpi,
        const internal::WindowBasicStyle& basic_style,
        const internal::WindowExtendedStyle& extend_style) noexcept;

    Point GetContentOriginInScreen() const noexcept;

private:
    Window& window_;
    std::optional<float> min_width_;
    std::optional<float> max_width_;
    std::optional<float> min_height_;
    std::optional<float> max_height_;
};

}