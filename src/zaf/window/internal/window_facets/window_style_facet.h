#pragma once

#include <zaf/base/non_copyable.h>
#include <zaf/window/activate_options.h>

namespace zaf {
class Window;
class WindowClass;
}

namespace zaf::internal {

class WindowStyleFacet : public NonCopyableNonMovable {
public:
    WindowStyleFacet(Window& window, std::shared_ptr<WindowClass> window_class);

    const std::shared_ptr<WindowClass>& Class() const noexcept {
        return class_;
    }

    std::wstring Title() const;
    void SetTitle(const std::wstring& title);

    bool IsPopup() const noexcept;
    void SetIsPopup(bool is_popup);

    bool HasBorder() const noexcept;
    void SetHasBorder(bool has_border);

    bool HasTitleBar() const noexcept;
    void SetHasTitleBar(bool has_title_bar);

    bool HasSystemMenu() const noexcept;
    void SetHasSystemMenu(bool has_system_menu);

    bool IsSizable() const noexcept;
    void SetIsSizable(bool is_sizable);

    bool CanMaximize() const noexcept;
    void SetCanMaximize(bool can_maximize);

    bool CanMinimize() const noexcept;
    void SetCanMinimize(bool can_minimize);

    bool IsToolWindow() const noexcept;
    void SetIsToolWindow(bool is_tool_window);

    bool IsTopmost() const noexcept;
    void SetIsTopmost(bool is_topmost);

    zaf::ActivateOptions ActivateOptions() const noexcept;
    void SetActivateOptions(zaf::ActivateOptions options);

private:
    template<typename PROPERTY>
    bool HasStyleProperty(PROPERTY property) const noexcept;
    template<typename PROPERTY>
    void SetStyleProperty(PROPERTY property, bool enable, bool can_set_if_has_handle);

private:
    Window& window_;

    std::shared_ptr<WindowClass> class_;
    zaf::ActivateOptions activate_options_{ zaf::ActivateOptions::Normal };
};

}