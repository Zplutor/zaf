#pragma once

/**
@file
    Defines the `zaf::WindowClass` class.
*/

#include <zaf/base/non_copyable.h>
#include <zaf/rx/subject/single_subject.h>
#include <zaf/window/window_class_properties.h>

namespace zaf {

class WindowClassRegistry;

/**
Represents a window class.

@details
    Window classes are managed by the `zaf::WindowClassRegistry` class.
*/
class WindowClass : NonCopyableNonMovable {
public:
    /**
    Destruct the window class, unregistering it if it has been registered.
    */
    ~WindowClass();

    /**
    Gets the name of the window class.
    */
    const std::wstring& Name() const noexcept {
        return properties_.Name();
    }

    /**
    Gets the original properties of the window class.

    @details
        Properties of a window class may be changed after it is registered. This method returns
        the original properties used to register the window class.
    */
    const WindowClassProperties& OriginalProperties() const noexcept {
        return properties_;
    }

    /**
    Gets the atom referring to the current window class.

    @throw zaf::Win32Error
        Thrown if fails to register the window class.

    @details
        The window class will be registered when this method is called for the first time.
    */
    ATOM GetAtom() const;

private:
    explicit WindowClass(const WindowClassProperties& properties);

    rx::Single<std::wstring> DestroyEvent() const noexcept {
        return destroyed_event_.AsSingle();
    }

private:
    const WindowClassProperties properties_;
    mutable ATOM atom_{};
    rx::SingleSubject<std::wstring> destroyed_event_;

    friend class WindowClassRegistry;
};

}