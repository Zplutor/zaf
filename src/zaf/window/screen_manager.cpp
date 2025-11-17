#include <zaf/window/screen_manager.h>
#include <zaf/application.h>

namespace zaf {

ScreenManager& ScreenManager::Instance() noexcept {
    return Application::Instance().GetScreenManager();
}


ScreenManager::ScreenManager() {
    UpdateScreens();
}


void ScreenManager::UpdateScreens() {
    auto new_screens = DiffScreens(screens_);
    screens_ = std::move(new_screens);
    primary_screen_ = FindPrimaryScreen(screens_);
}


std::vector<std::shared_ptr<Screen>> ScreenManager::DiffScreens(
    const std::vector<std::shared_ptr<Screen>>& current_screens) {

    std::vector<HMONITOR> handles;
    EnumDisplayMonitors(
        nullptr,
        nullptr,
        [](HMONITOR monitor, HDC, LPRECT, LPARAM data) -> BOOL {
            auto handles = reinterpret_cast<std::vector<HMONITOR>*>(data);
            handles->push_back(monitor);
            return TRUE;
        },
        reinterpret_cast<LPARAM>(&handles));

    auto result = current_screens;
    auto screen_iterator = result.begin();
    while (screen_iterator != result.end()) {

        auto handle_iterator = std::find(
            handles.begin(),
            handles.end(),
            (*screen_iterator)->Handle());

        if (handle_iterator == handles.end()) {
            screen_iterator = result.erase(screen_iterator);
        }
        else {

            handles.erase(handle_iterator);

            (*screen_iterator)->UpdateMonitorInfo();
            ++screen_iterator;
        }
    }

    for (auto handle : handles) {
        auto screen = std::make_shared<Screen>(handle);
        result.push_back(screen);
    }
    return result;
}


std::shared_ptr<Screen> ScreenManager::FindPrimaryScreen(
    const std::vector<std::shared_ptr<Screen>>& screens) noexcept {

    for (const auto& each_screen : screens) {
        if (each_screen->IsPrimary()) {
            return each_screen;
        }
    }

    if (!screens.empty()) {
        return screens.front();
    }

    return nullptr;
}

}