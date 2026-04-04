#pragma once

#include <zaf/base/non_copyable.h>
#include <zaf/window/find_screen_option.h>
#include <zaf/window/screen.h>

namespace zaf {

class Application;

class ScreenManager : NonCopyableNonMovable {
public:
    static ScreenManager& Instance() noexcept;

public:
    const std::vector<std::shared_ptr<Screen>>& AllScreens() const noexcept {
        return screens_;
    }

    const std::shared_ptr<Screen>& PrimaryScreen() const noexcept {
        return primary_screen_;
    }

    std::shared_ptr<Screen> ScreenFromHandle(HMONITOR handle) const noexcept;

    std::shared_ptr<Screen> ScreenFromGlobalPosition(
        const Point& global_position,
        FindScreenOption option = FindScreenOption::DefaultToNull) const noexcept;

private:
    friend class Application;

private:
    ScreenManager();
    void UpdateScreens();
    static std::vector<std::shared_ptr<Screen>> DiffScreens(
        const std::vector<std::shared_ptr<Screen>>& current_screens);
    static std::shared_ptr<Screen> FindPrimaryScreen(
        const std::vector<std::shared_ptr<Screen>>& screens) noexcept;

private:
    std::vector<std::shared_ptr<Screen>> screens_;
    std::shared_ptr<Screen> primary_screen_;
};

}