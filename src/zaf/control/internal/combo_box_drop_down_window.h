#pragma once

#include <zaf/control/control.h>
#include <zaf/window/window.h>

namespace zaf {

class ListControl;

namespace internal {

class ComboBoxDropDownWindow : public Window {
public:
    void Initialize() override;

    void SetListControl(const std::shared_ptr<ListControl>& list_control);

protected:
    void WindowShow();
    void CapturingMouseControlChange(const std::shared_ptr<Control>& previous_control);

private:
    class Container : public Control {
    protected:
        void MouseUp(const Point& position, const MouseMessage& message) override;
    };

private:
    std::shared_ptr<Container> container_;
    std::shared_ptr<ListControl> list_control_;
};

}
}