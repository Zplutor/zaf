#pragma once

#include <zaf/control/textual_control.h>

namespace zaf {

class ListItem : public TextualControl {
public:
    /**
     Get a value indicating that whether the item is selected.
     */
    bool IsSelected() const {
        return is_selected_;
    }

    /**
     Set a value indicating that whether the item is selected.
     */
    void SetIsSelected(bool is_selected) {
        if (is_selected_ != is_selected) {
            is_selected_ = is_selected;
            NeedRepaint();
        }
    }

protected:
    void Initialize() override;

private:
    bool is_selected_{};
};

}