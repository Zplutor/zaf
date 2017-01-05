#pragma once

#include <zaf/window/window.h>

namespace zaf {

class ListControl;

namespace internal {

class ComboBoxDropDownWindow : public Window {
public:
    void Initialize() override;

    void SetListControl(const std::shared_ptr<ListControl>& list_control);

private:
    std::shared_ptr<ListControl> list_control_;
};

}
}