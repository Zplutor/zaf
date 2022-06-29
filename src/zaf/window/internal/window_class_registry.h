#pragma once

#include <Windows.h>
#include <map>
#include <string>
#include <zaf/base/non_copyable.h>

namespace zaf::internal {

class WindowClass;

class WindowClassRegistry : NonCopyable {
public:
    WindowClassRegistry(HICON default_icon, HICON default_small_icon);

    std::shared_ptr<WindowClass> GetWindowClass(const std::wstring& class_name);

private:
    friend class WindowClass;

    std::map<std::wstring, std::weak_ptr<WindowClass>> classes_;
    HICON default_icon_{};
    HICON default_small_icon_{};
};


class WindowClass : NonCopyable {
public:
    WindowClass(WindowClassRegistry& registry, const std::wstring& class_name);
    ~WindowClass();

    const std::wstring& Name() const {
        return class_name_;
    }

    ATOM GetAtom();

private:
    WindowClassRegistry& registry_;
    std::wstring class_name_;
    ATOM atom_{};
};

}