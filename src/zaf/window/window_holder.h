#pragma once

#include <memory>

namespace zaf {

class Window;

/**
An auto class to destroy the handle of specified window on destruction.
*/
class WindowHolder {
public:
    explicit WindowHolder(const std::shared_ptr<zaf::Window>& window);
    ~WindowHolder();

    /**
    Gets the window attached with this holder. Might be null if the handle of window has been 
    destroyed.
    */
    const std::shared_ptr<zaf::Window>& Window() const {
        return window_;
    }

private:
    friend class Window;

    void Detach() noexcept {
        window_.reset();
    }

private:
    std::shared_ptr<zaf::Window> window_;
};

}