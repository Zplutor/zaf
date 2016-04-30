#pragma once

#include <memory>
#include <zaf/base/event.h>

namespace zaf {

class SelfScrollingControl {
public:
    typedef Event<SelfScrollingControl&> ScrollBarChangeEvent;
    typedef Event<SelfScrollingControl&, bool> ScrollValuesChangeEvent;

public:
    SelfScrollingControl() { } 
    virtual ~SelfScrollingControl() { }

    virtual void SetAllowVerticalScroll(bool allow) = 0;
    virtual void SetAllowHorizontalScroll(bool allow) = 0;

    virtual void SetAutoHideScrollBars(bool auto_hide) = 0;

    virtual bool CanShowVerticalScrollBar() = 0;
    virtual bool CanShowHorizontalScrollBar() = 0;

    virtual bool CanEnableVerticalScrollBar() = 0;
    virtual bool CanEnableHorizontalScrollBar() = 0;

    virtual void GetVerticalScrollValues(int& current_value, int& min_value, int& max_value) = 0;
    virtual void GetHorizontalScrollValues(int& current_value, int& min_value, int& max_value) = 0;

    virtual ScrollBarChangeEvent::Proxy GetScrollBarChangeEvent() = 0;
    virtual ScrollValuesChangeEvent::Proxy GetScrollValuesChangeEvent() = 0;

    virtual void VerticallyScroll(int new_value) = 0;
    virtual void HorizontallyScroll(int new_value) = 0;

private:
    SelfScrollingControl(const SelfScrollingControl&) = delete;
    SelfScrollingControl& operator=(const SelfScrollingControl&) = delete;
};

}