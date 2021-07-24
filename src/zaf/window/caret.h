#pragma once

#include <memory>
#include <zaf/graphic/rect.h>
#include <zaf/rx/subscription_host.h>

namespace zaf {

class Canvas;
class Timer;
class Window;

/**
 Represents an input caret in a window.

 There is no need to create instance of Caret by yourself. Call Window::Caret method
 to get the unique caret in the window.
 */
class Caret : public SubscriptionHost {
public:
    /**
     Construct the instance.
     */
    Caret();

    /**
     Destruct the instance.
     */
    ~Caret();

    /**
     Get the caret's rect.

     The default rect is empty, you should use SetRect method to set a 
     appropriate rect.
     */
    const Rect& GetRect() const {
        return rect_;
    }

    /**
     Set the caret's rect.
     */
    void SetRect(const Rect& rect);

    /**
     Set the caret's position.

     This is a shortcut of SetRect.
     */
    void SetPosition(const Point& position) {
        SetRect(Rect(position, GetRect().size));
    }

    /**
     Set the caret's size.

     This is a shortcut of SetRect.
     */
    void SetSize(const Size& size) {
        SetRect(Rect(GetRect().position, size));
    }

    /**
     Show the caret.

     The caret starts blinking.
     */
    void Show();

    /**
     Hide the caret.

     The caret stops blinking.
     */
    void Hide();

private:
    friend class Window;

    void SetWindow(const std::shared_ptr<Window>& window) {
        window_ = window;
    }

    void Repaint(Canvas& canvas);

private:
    bool IsShown() const {
        return blink_timer_ != nullptr;
    }

    void CreateSystemCaret();
    void DestroySystemCaret();

    void NeedRepaint(bool bling_visible);

private:
    std::weak_ptr<Window> window_;
    Rect rect_;
    bool is_visible_;
    std::shared_ptr<Timer> blink_timer_;
};

}