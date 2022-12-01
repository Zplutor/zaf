#pragma once

#include <zaf/graphic/rect.h>

namespace zaf {

class Control;

class ControlEventInfo {
public:
    ControlEventInfo(const std::shared_ptr<Control>& control) : control_(control) {

    }

    const std::shared_ptr<Control>& Control() const {
        return control_;
    }

private:
    std::shared_ptr<zaf::Control> control_;
};


class ControlClickInfo : public ControlEventInfo {
public:
    using ControlEventInfo::ControlEventInfo;
};


class ControlDoubleClickInfo : public ControlEventInfo {
public:
    ControlDoubleClickInfo(const std::shared_ptr<zaf::Control>& control, const Point& position) :
        ControlEventInfo(control), 
        position_(position) {

    }

    const Point& Position() const {
        return position_;
    }

private:
    Point position_;
};


class ControlFocusChangeInfo : public ControlEventInfo {
public:
    using ControlEventInfo::ControlEventInfo;
};


class ControlMouseEnterInfo : public ControlEventInfo {
public:
    ControlMouseEnterInfo(
        const std::shared_ptr<zaf::Control>& control,
        const std::shared_ptr<zaf::Control>& entered_control)
        :
        ControlEventInfo(control),
        entered_control_(entered_control) {

    }

    const std::shared_ptr<zaf::Control>& EnteredControl() const {
        return entered_control_;
    }

private:
    std::shared_ptr<zaf::Control> entered_control_;
};


class ControlMouseHoverInfo : public ControlEventInfo {
public:
    ControlMouseHoverInfo(
        const std::shared_ptr<zaf::Control>& control,
        const std::shared_ptr<zaf::Control>& hovered_control)
        :
        ControlEventInfo(control),
        hovered_control_(hovered_control) {

    }

    const std::shared_ptr<zaf::Control>& HoveredControl() const {
        return hovered_control_;
    }

private:
    std::shared_ptr<zaf::Control> hovered_control_;
};


class ControlMouseLeaveInfo : public ControlEventInfo {
public:
    ControlMouseLeaveInfo(
        const std::shared_ptr<zaf::Control>& control,
        const std::shared_ptr<zaf::Control>& leaved_control)
        :
        ControlEventInfo(control),
        leaved_control_(leaved_control) {

    }

    const std::shared_ptr<zaf::Control>& LeavedControl() const {
        return leaved_control_;
    }

private:
    std::shared_ptr<zaf::Control> leaved_control_;
};


class ControlRectChangedInfo : public ControlEventInfo {
public:
    ControlRectChangedInfo(
        const std::shared_ptr<zaf::Control>& control,
        const Rect& previous_rect)
        : 
        ControlEventInfo(control),
        previous_rect_(previous_rect) {

    }

    const zaf::Rect& PreviousRect() const {
        return previous_rect_;
    }

private:
    zaf::Rect previous_rect_;
};


class ControlParentChangedInfo : public ControlEventInfo {
public:
    ControlParentChangedInfo(
        const std::shared_ptr<zaf::Control>& control,
        const std::shared_ptr<zaf::Control>& previous_parent) 
        : 
        ControlEventInfo(control), 
        previous_parent_(previous_parent) {
    
    }

    const std::shared_ptr<zaf::Control>& PreviousParent() const {
        return previous_parent_;
    }

private:
    std::shared_ptr<zaf::Control> previous_parent_;
};

}