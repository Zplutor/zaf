#pragma once

#include <zaf/base/event/event_info.h>
#include <zaf/clipboard/data_object.h>

namespace zaf {

class TextBox;

namespace textual {

class PastingInfo : public EventInfo {
public:
    PastingInfo(
        std::shared_ptr<TextBox> source,
        clipboard::DataObject data_object);

    const clipboard::DataObject& DataObject() const {
        return data_object_;
    }

    /**
    Indicates whether the paste operation is handled.
    */
    bool IsHandled() const {
        return *is_handled_;
    }

    /**
    Marks the paste operation as handled.
    */
    void MarkAsHandled() const {
        *is_handled_ = true;
    }

private:
    std::shared_ptr<bool> is_handled_;
    clipboard::DataObject data_object_;
};

}
}