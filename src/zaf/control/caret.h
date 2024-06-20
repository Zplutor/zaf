#pragma once

#include <zaf/base/non_copyable.h>
#include <zaf/control/control.h>
#include <zaf/object/object.h>
#include <zaf/object/property_support.h>

namespace zaf {

class Canvas;

class Caret : public Object, NonCopyableNonMovable {
public:
    ZAF_OBJECT;

    /**
    Construct a caret which belongs to the specified control.
    */
    explicit Caret(const std::weak_ptr<Control>& owner);
    ~Caret();

    const zaf::Rect& Rect() const {
        return rect_;
    }

    /**
    Sets rect of the caret.

    Note that the position of rect is in owner content coordinate.
    */
    void SetRect(const zaf::Rect& rect);

    const Point& Position() const {
        return rect_.position;
    }

    /**
    Sets position of the caret.

    Note that the position is in owner content coordinate.
    */
    void SetPosition(const Point& position) {
        SetRect(zaf::Rect{ position, rect_.size });
    }

    const zaf::Size& Size() const {
        return rect_.size;
    }

    void SetSize(const zaf::Size& size) {
        SetRect(zaf::Rect{ rect_.position, size });
    }

    bool IsVisible() const {
        return is_visible_;
    }

    void SetIsVisible(bool is_visible);

    /**
    Paints the caret.

    @param owner_canvas
        The canvas of owner. It should have been pushed a region of content area.

    @param dirty_rect
        A rectangle area that need to be painted, in owner content coordinate.
    */
    void Paint(Canvas& owner_canvas, const zaf::Rect& dirty_rect);

private:
    void ShowCaret();
    void HideCaret();
    void NeedRepaint();
    void CreateSystemCaret();
    void DestroySystemCaret();

private:
    std::weak_ptr<Control> owner_;
    zaf::Rect rect_{};
    bool is_visible_{};

    Subscription blink_timer_subscription_;
    bool is_blink_on_{};
};

ZAF_OBJECT_BEGIN(Caret);
ZAF_OBJECT_PROPERTY(Rect);
ZAF_OBJECT_PROPERTY(Position);
ZAF_OBJECT_PROPERTY(Size);
ZAF_OBJECT_PROPERTY(IsVisible);
ZAF_OBJECT_END;

}