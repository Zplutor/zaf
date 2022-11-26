#pragma once

#include <zaf/base/non_copyable.h>
#include <zaf/control/control.h>
#include <zaf/object/object.h>

namespace zaf {

class Canvas;

class Caret : public Object, NonCopyable {
public:
    ZAF_DECLARE_TYPE;

    explicit Caret(const std::weak_ptr<Control>& owner);
    ~Caret();

    const zaf::Rect& Rect() const {
        return rect_;
    }

    void SetRect(const zaf::Rect& rect);

    const Point& Position() const {
        return rect_.position;
    }

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

}