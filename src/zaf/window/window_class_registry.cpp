#include <zaf/window/window_class_registry.h>
#include <zaf/application.h>
#include <zaf/window/window.h>

namespace zaf {

WindowClassRegistry& WindowClassRegistry::Instance() {
    return Application::Instance().GetWindowClassRegistry();
}


WindowClassRegistry::WindowClassRegistry(HICON default_icon, HICON default_small_icon) {

    WindowClassProperties default_class_properties;
    default_class_properties.SetName(DefaultWindowClassName);
    default_class_properties.SetStyle(
        WindowClassStyle::HorizontalRedraw | WindowClassStyle::VerticalRedraw);
    default_class_properties.SetWindowProcedure(Window::WindowProcedure);
    default_class_properties.SetWindowExtraBytes(sizeof(LONG_PTR));
    default_class_properties.SetCursor(LoadCursor(NULL, IDI_APPLICATION));
    default_class_properties.SetBackgroundBrush(reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1));
    default_class_properties.SetIcon(default_icon);
    default_class_properties.SetSmallIcon(default_small_icon);

    default_class_ = RegisterWindowClass(default_class_properties);
}


WindowClassRegistry::~WindowClassRegistry() {

    //default_class_ must be destroyed before class_map_.
    default_class_.reset();
    class_map_.clear();
}


std::shared_ptr<WindowClass> WindowClassRegistry::GetDefaultWindowClass() {
    return default_class_;
}


WindowClassProperties WindowClassRegistry::GetDefaultWindowClassProperties() {
    return default_class_->OriginalProperties();
}


std::shared_ptr<WindowClass> WindowClassRegistry::GetOrRegisterWindowClass(
    std::wstring_view class_name,
    std::function<void(WindowClassProperties&)> class_properties_getter) {

    auto result = GetWindowClass(class_name);
    if (result) {
        return result;
    }

    WindowClassProperties class_properties = default_class_->OriginalProperties();
    class_properties.SetName(class_name);
    if (class_properties_getter) {
        class_properties_getter(class_properties);
    }
    return RegisterWindowClass(class_properties);
}


std::shared_ptr<WindowClass> WindowClassRegistry::GetWindowClass(std::wstring_view class_name) {

    auto iterator = class_map_.find(class_name);
    if (iterator == class_map_.end()) {
        return nullptr;
    }

    auto result = iterator->second.lock();
    if (!result) {
        class_map_.erase(iterator);
        return nullptr;
    }

    return result;
}


std::shared_ptr<WindowClass> WindowClassRegistry::RegisterWindowClass(
    const WindowClassProperties& properties) {

    std::shared_ptr<WindowClass> window_class{ new WindowClass(properties) };
    Disposables() += window_class->DestroyEvent().Subscribe(
        [this](const std::wstring& class_name) {
        class_map_.erase(class_name);
    });

    class_map_[window_class->Name()] = window_class;
    return window_class;
}

}