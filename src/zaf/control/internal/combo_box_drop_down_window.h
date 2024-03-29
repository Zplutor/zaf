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
    void OnShow(const ShowInfo& event_info) override;
    void OnMouseCaptureControlChanged(const MouseCaptureControlChangedInfo& event_info) override;

private:
    class Container : public Control {
    protected:
        void OnMouseCaptured(const MouseCapturedInfo& event_info) override;
        void OnMouseReleased(const MouseReleasedInfo& event_info) override;
        void OnMouseMove(const MouseMoveInfo& event_info) override;
        void OnMouseUp(const MouseUpInfo& event_info) override;

    private:
        HCURSOR originally_cursor_ = nullptr;
    };

private:
    std::shared_ptr<Container> container_;
    std::shared_ptr<ListControl> list_control_;
};

}
}