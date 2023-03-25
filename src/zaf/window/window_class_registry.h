#pragma once

#include <zaf/base/non_copyable.h>
#include <zaf/rx/subscription_host.h>
#include <zaf/window/window_class.h>

namespace zaf {

class Application;

class WindowClassRegistry : SubscriptionHost, NonCopyable {
public:
    static WindowClassRegistry& Instance();

public:
    static constexpr const wchar_t* DefaultWindowClassName = L"ZafDef";

public:
    ~WindowClassRegistry();

    std::shared_ptr<WindowClass> GetDefaultWindowClass();
    WindowClassProperties GetDefaultWindowClassProperties();

    std::shared_ptr<WindowClass> GetOrRegisterWindowClass(
        std::wstring_view class_name,
        std::function<void(WindowClassProperties&)> class_properties_getter);

private:
    friend class Application;
    friend class WindowClass;

    WindowClassRegistry(HICON default_icon, HICON default_small_icon);

private:
    std::shared_ptr<WindowClass> GetWindowClass(std::wstring_view class_name);
    std::shared_ptr<WindowClass> RegisterWindowClass(const WindowClassProperties& properties);

private:
    std::map<std::wstring_view, std::weak_ptr<WindowClass>> class_map_;
    std::shared_ptr<WindowClass> default_class_;
};

}