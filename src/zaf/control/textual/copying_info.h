#pragma once

/**
@file
    Defines the zaf::textual::CopyingInfo class.
*/

#include <zaf/base/event/event_info.h>
#include <zaf/base/range.h>

namespace zaf {

class TextBox;

namespace textual {

/**
Contains information for the copying event of TextBox.
*/
class CopyingInfo : public EventInfo {
public:
    /**
    Constructs the instance.

    @param source
        The TextBox instance from which the event is raised.

    @param selection_range
        The selected range of the text to be copied.

    @throw std::bad_alloc
    */
    CopyingInfo(std::shared_ptr<TextBox> source, const Range& selection_range);

    /**
    Gets the selected range of the text to be copied.
    */
    const Range& SelectionRange() const {
        return selection_range_;
    }

    /**
    Indicates whether the copy operation is handled.
    */
    bool IsHandled() const {
        return *is_handled_;
    }

    /**
    Marks the copy operation as handled.
    */
    void MarkAsHandled() const {
        *is_handled_ = true;
    }

private:
    std::shared_ptr<bool> is_handled_;
    Range selection_range_;
};

}
}