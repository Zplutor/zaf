#pragma once

#include <zaf/base/event/event_info.h>

namespace zaf::textual {

class InlineObject;

class AttachedInfo : public EventInfo {
public:
    explicit AttachedInfo(std::shared_ptr<InlineObject> source);
};


class DetachedInfo : public EventInfo {
public:
    explicit DetachedInfo(std::shared_ptr<InlineObject> source);
};

}