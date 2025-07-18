#pragma once

#include <memory>
#include <zaf/rx/observable.h>

namespace zaf {

class SelfScrollControlScrollBarChangeInfo;
class SelfScrollControlScrollValuesChangeInfo;

/**
 Provides an interface for self scrolling controls to interact with scrollable controls.

 Any class that inherits SelfScrollControl is regarded as a control that can manage its
 own scrolling action. When it is set as the scroll content control to a scrollable control, 
 the later queries and changes the scroll bars information via this interface.
 */
class SelfScrollControl {
public:
    SelfScrollControl() { } 
    virtual ~SelfScrollControl() { }

    /**
     Set a value indicating that whether the scrollable control allows vertical scrolling.
     */
    virtual void SetAllowVerticalScroll(bool allow) = 0;

    /**
     Set a value indicating that whether the scrollable control allows horizontal scrolling.
     */
    virtual void SetAllowHorizontalScroll(bool allow) = 0;

    /**
     Set a value indicating that whether the scrollable control auto hides scroll bars when 
     they are no needed.
     */
    virtual void SetAutoHideScrollBars(bool auto_hide) = 0;

    /**
     Get a value indicating that whether the self scrolling control can show vertical scroll bar.
     */
    virtual bool CanShowVerticalScrollBar() = 0;

    /**
     Get a value indicating that whether the self scrolling control can show horizontal scroll bar.
     */
    virtual bool CanShowHorizontalScrollBar() = 0;

    /**
     Get a value indicating that whether the self scrolling control can enable vertical scroll bar.
     */
    virtual bool CanEnableVerticalScrollBar() = 0;

    /**
     Get a value indicating that whether the self scrolling control can enable horizontal scroll bar.
     */
    virtual bool CanEnableHorizontalScrollBar() = 0;

    /**
     Get vertical scroll values of the self scrolling control.
     */
    virtual void GetVerticalScrollValues(
        int& current_value, 
        int& min_value, 
        int& max_value,
        int& page_value) const = 0;

    /**
     Get horizontal scroll values of the self scrolling control.
     */
    virtual void GetHorizontalScrollValues(
        int& current_value, 
        int& min_value, 
        int& max_value,
        int& page_value) const = 0;

    /**
     Get scroll bar change event of the self scrolling control.

     This event should be raised when the visibility or usability of scroll bars changed.
     */
    virtual rx::Observable<SelfScrollControlScrollBarChangeInfo> ScrollBarChangeEvent() = 0;

    /**
     Get scroll value change event of the self scrolling control.

     This event should be raise when the scroll values changed.
     */
    virtual rx::Observable<SelfScrollControlScrollValuesChangeInfo> ScrollValuesChangeEvent() = 0;

    /**
     Scroll the content by changing the vertical scroll value.
     */
    virtual void VerticallyScroll(int new_value) = 0;

    /**
     Scroll the content by changing the horizontal scroll value. 
     */
    virtual void HorizontallyScroll(int new_value) = 0;

private:
    SelfScrollControl(const SelfScrollControl&) = delete;
    SelfScrollControl& operator=(const SelfScrollControl&) = delete;
};


class SelfScrollControlScrollBarChangeInfo {
public:
    SelfScrollControl* self_scroll_control{};
};


class SelfScrollControlScrollValuesChangeInfo {
public:
    SelfScrollControl* self_scroll_control{};
    bool is_horizontal{};
};

}