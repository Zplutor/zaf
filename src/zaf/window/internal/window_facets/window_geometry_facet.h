#pragma once

#include <zaf/base/non_copyable.h>
#include <zaf/graphic/frame.h>
#include <zaf/graphic/rect.h>

namespace zaf {
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

    Frame GetWindowFrame(
        const internal::WindowBasicStyle& basic_style,
        const internal::WindowExtendedStyle& extend_style) const noexcept;

    zaf::Size AdjustContentSizeToWindowSize(
        const zaf::Size& content_size,
        const internal::WindowBasicStyle& basic_style,
        const internal::WindowExtendedStyle& extend_style) const noexcept;

private:
    zaf::Size ClampSize(const zaf::Size& size) const noexcept;
    zaf::Rect AdjustContentRectToWindowRect(
        const zaf::Rect& content_rect,
        const internal::WindowBasicStyle& basic_style,
        const internal::WindowExtendedStyle& extend_style) const noexcept;

private:
    Window& window_;
    std::optional<float> min_width_;
    std::optional<float> max_width_;
    std::optional<float> min_height_;
    std::optional<float> max_height_;
};

}