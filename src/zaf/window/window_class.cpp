#include <zaf/window/window_class.h>
#include <zaf/application.h>
#include <zaf/base/error/win32_error.h>
#include <zaf/window/window_class_registry.h>

namespace zaf {

WindowClass::WindowClass(const WindowClassProperties& properties) : properties_(properties) {

}


WindowClass::~WindowClass() {

    if (atom_) {
        UnregisterClass(reinterpret_cast<LPCWSTR>(atom_), nullptr);
    }

    destroyed_event_.AsObserver().OnNext(properties_.Name());
}


ATOM WindowClass::GetAtom() {

    if (!atom_) {

        atom_ = RegisterClassEx(&properties_.Inner());
        if (!atom_) {
            ZAF_THROW_WIN32_ERROR(GetLastError());
        }
    }

    return atom_;
}

}