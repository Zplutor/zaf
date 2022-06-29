#include <zaf/window/internal/window_class_registry.h>
#include <zaf/application.h>
#include <zaf/base/error/system_error.h>
#include <zaf/window/window.h>

namespace zaf::internal {

WindowClassRegistry::WindowClassRegistry(HICON default_icon, HICON default_small_icon) : 
    default_icon_(default_icon), 
    default_small_icon_(default_small_icon) {

}


std::shared_ptr<WindowClass> WindowClassRegistry::GetWindowClass(const std::wstring& class_name) {

    auto& window_class = classes_[class_name];

    auto result = window_class.lock();
    if (!result) {

        result = std::make_shared<WindowClass>(*this, class_name);
        window_class = result;
    }

    return result;
}


WindowClass::WindowClass(WindowClassRegistry& registry, const std::wstring& class_name) :
    registry_(registry),
    class_name_(class_name) {

}


WindowClass::~WindowClass() {

    if (atom_) {
        UnregisterClass(reinterpret_cast<LPCWSTR>(atom_), nullptr);
    }

    registry_.classes_.erase(class_name_);
}


ATOM WindowClass::GetAtom() {

    if (atom_) {
        return atom_;
    }

    WNDCLASSEX default_class = { 0 };
    default_class.cbSize = sizeof(default_class);
    default_class.style = CS_HREDRAW | CS_VREDRAW;
    default_class.lpfnWndProc = Window::WindowProcedure;
    default_class.cbClsExtra = 0;
    default_class.cbWndExtra = sizeof(LONG_PTR);
    default_class.hInstance = NULL;
    default_class.hIcon = registry_.default_icon_;
    default_class.hCursor = LoadCursor(NULL, IDI_APPLICATION);
    default_class.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
    default_class.lpszMenuName = nullptr;
    default_class.lpszClassName = class_name_.c_str();
    default_class.hIconSm = registry_.default_small_icon_;

    atom_ = RegisterClassEx(&default_class);
    if (!atom_) {
        ZAF_THROW_SYSTEM_ERROR(GetLastError());
    }

    return atom_;
}

}