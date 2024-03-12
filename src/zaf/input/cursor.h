#pragma once

#include <Windows.h>

namespace zaf {

class Cursor {
public:
    static Cursor Normal() noexcept {
        return Cursor{ LoadCursor(nullptr, IDC_ARROW) };
    }

    static Cursor IBeam() noexcept {
        return Cursor{ LoadCursor(nullptr, IDC_IBEAM) };
    }

public:
    Cursor() noexcept = default;

    explicit Cursor(HCURSOR handle) noexcept : handle_(handle) {

    }

    Cursor(const Cursor& other) noexcept = default;
    Cursor& operator=(const Cursor& other) noexcept = default;

    Cursor(Cursor&& other) noexcept {
        handle_ = other.handle_;
        other.handle_ = nullptr;
    }

    Cursor& operator=(Cursor&& other) noexcept {
        handle_ = other.handle_;
        other.handle_ = nullptr;
        return *this;
    }

    HCURSOR Handle() const noexcept {
        return handle_;
    }

private:
    HCURSOR handle_{};
};

}