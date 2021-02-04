#pragma once

#include <zaf/control/control.h>
#include <zaf/window/window.h>

namespace zaf {

class ListControl;

namespace internal {

class ComboBoxDropDownWindow : public Window {
public:
    void SetListControl(const std::shared_ptr<ListControl>& list_control);

protected:
    void Initialize() override;
    void WindowShow() override;
    void WindowDestroy(HWND handle) override;
    void CapturingMouseControlChange(const std::shared_ptr<Control>& previous_control);

private:
    class Container : public Control {
    protected:
        void OnMouseCapture() override;
        void OnMouseRelease() override;
        bool OnMouseMove(const Point& position, const MouseMessage& message) override;
        bool OnMouseUp(const Point& position, const MouseMessage& message) override;

    private:
        HCURSOR originally_cursor_ = nullptr;
    };

private:
    std::shared_ptr<Container> container_;
    std::shared_ptr<ListControl> list_control_;
};

}
}