#pragma once

#include <zaf/base/non_copyable.h>
#include <zaf/rx/subject.h>
#include <zaf/window/window_class_properties.h>

namespace zaf {

class WindowClassRegistry;

class WindowClass : NonCopyableNonMovable {
public:
    ~WindowClass();

    const std::wstring& Name() const {
        return properties_.Name();
    }

    const WindowClassProperties& OriginalProperties() const {
        return properties_;
    }

    ATOM GetAtom();

private:
    friend class WindowClassRegistry;

    explicit WindowClass(const WindowClassProperties& properties);

    Observable<std::wstring> DestroyEvent() {
        return destroyed_event_.GetObservable();
    }

private:
    WindowClassProperties properties_;
    ATOM atom_{};
    Subject<std::wstring> destroyed_event_;
};

}