#pragma once

#include <zaf/base/event/event_info.h>
#include <zaf/window/message/ime_message.h>

namespace zaf {

class Control;

class IMEStartCompositionInfo : public EventInfo {
public:
    IMEStartCompositionInfo(std::shared_ptr<Control> source, const zaf::Message& message);

    const zaf::Message& Message() const {
        return message_;
    }

private:
    zaf::Message message_;
};


class IMECompositionInfo : public EventInfo {
public:
    IMECompositionInfo(std::shared_ptr<Control> source, const Message& message);

    IMECompositionMessage Message() const {
        return IMECompositionMessage{ message_ };
    }

private:
    zaf::Message message_;
};


class IMEEndCompositionInfo : public EventInfo {
public:
    IMEEndCompositionInfo(std::shared_ptr<Control> source, const Message& message);

    const zaf::Message& Message() const {
        return message_;
    }

private:
    zaf::Message message_;
};

}