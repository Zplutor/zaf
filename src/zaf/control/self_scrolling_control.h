#pragma once

#include <memory>
#include <zaf/base/event.h>

namespace zaf {

class ScrollableControl;

class ScrollInformation {
public:
    ScrollInformation() : 
        is_enabled(false),
        current_value(0), 
        maximum_value(0), 
        minimum_value(0) {
    
    }

    bool is_enabled;
    int current_value;
    int maximum_value;
    int minimum_value;
};

class SelfScrollingControl {
public:
    typedef Event<SelfScrollingControl&> ScrollInformationChangeEvent;

public:
    virtual ~SelfScrollingControl() { }

    virtual void SetIsVerticalScrollEnabled(bool is_enabled) = 0;
    virtual void SetIsHorizontalScrollEnabled(bool is_enabled) = 0;

    virtual void GetVerticalScrollInformation(ScrollInformation& information) = 0;
    virtual void GetHorizontalScrollInformation(ScrollInformation& information) = 0;

    virtual ScrollInformationChangeEvent::Proxy GetScrollInformationChangeEvent() = 0;

    virtual void VerticallyScroll(int value_changed) = 0;
    virtual void HorizontallyScroll(int value_changed) = 0;

private:
    SelfScrollingControl(const SelfScrollingControl&) = delete;
    SelfScrollingControl& operator=(const SelfScrollingControl&) = delete;
};

}