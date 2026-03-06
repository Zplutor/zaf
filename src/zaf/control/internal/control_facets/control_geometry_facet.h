#pragma once

#include <optional>
#include <zaf/base/non_copyable.h>
#include <zaf/control/anchor.h>
#include <zaf/graphic/frame.h>
#include <zaf/graphic/rect.h>

namespace zaf {
class Control;
}

namespace zaf::internal {

class ControlGeometryFacet : NonCopyableNonMovable {
public:
    explicit ControlGeometryFacet(Control& control) noexcept;

    const zaf::Rect& Rect() const noexcept;
    void SetRect(const zaf::Rect& rect);

    zaf::Rect RectInSelf() const noexcept;
    std::optional<zaf::Rect> RectInWindow() const noexcept;

    const Point& Position() const noexcept;
    void SetPosition(const Point& position);
    std::optional<zaf::Point> PositionInWindow() const noexcept;

    float X() const noexcept;
    void SetX(float x);
    float Y() const noexcept;
    void SetY(float y);

    const zaf::Size& Size() const noexcept;
    void SetSize(const zaf::Size& size);

    float Width() const noexcept;
    void SetWidth(float width);
    float Height() const noexcept;
    void SetHeight(float height);

    float MinWidth() const noexcept;
    void SetMinWidth(float min_width);
    float MaxWidth() const noexcept;
    void SetMaxWidth(float max_width);
    void SetFixedWidth(float width);

    float MinHeight() const noexcept;
    void SetMinHeight(float min_height);
    float MaxHeight() const noexcept;
    void SetMaxHeight(float max_height);
    void SetFixedHeight(float height);

    void SetFixedSize(const zaf::Size& size);

    zaf::Size CalculatePreferredSize() const;
    zaf::Size CalculatePreferredSize(const zaf::Size& bound_size) const;

    bool AutoWidth() const;
    void SetAutoWidth(bool value);
    bool AutoHeight() const;
    void SetAutoHeight(bool value);
    void SetAutoSize(bool value);

    float ClampWidth(float width) const noexcept;
    float ClampHeight(float height) const noexcept;
    zaf::Size ClampSize(const zaf::Size& size) const noexcept;

    zaf::Anchor Anchor() const;
    void SetAnchor(zaf::Anchor anchor);

    const Frame& Margin() const;
    void SetMargin(const Frame& margin);

    const Frame& Border() const;
    void SetBorder(const Frame& border);

    const Frame& Padding() const;
    void SetPadding(const Frame& padding);

    std::optional<zaf::Rect> ContentRectInWindow() const noexcept;
    zaf::Rect ContentRectInSelf() const noexcept;
    zaf::Rect ContentRect() const noexcept;
    zaf::Size ContentSize() const;

    void AutoResizeToPreferredSize();

private:
    void ApplyAutoSizeOnNewSize(
        zaf::Size& new_size, 
        std::optional<float>& new_fixed_width,
        std::optional<float>& new_fixed_height) const;

    void SetFixedWidthValue(float value) noexcept;
    void SetFixedHeightValue(float value) noexcept;
    zaf::Size CalculatePreferredSizeForAutoSize(const zaf::Size& control_size) const;

private:
    Control& control_;
    zaf::Rect rect_;
    zaf::Anchor anchor_{ zaf::Anchor::None };
    bool auto_width_{};
    bool auto_height_{};
    float min_width_{};
    float max_width_{ (std::numeric_limits<float>::max)() };
    float min_height_{};
    float max_height_{ (std::numeric_limits<float>::max)() };
    Frame margin_;
    Frame border_;
    Frame padding_;
};

}
