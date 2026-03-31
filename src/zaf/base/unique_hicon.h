#pragma once

#include <Windows.h>
#include <zaf/base/non_copyable.h>

namespace zaf {

class UniqueHICON : NonCopyable {
public:
    UniqueHICON() = default;
    explicit UniqueHICON(HICON hicon, bool is_shared = false) noexcept;
    ~UniqueHICON();

    UniqueHICON(UniqueHICON&& other) noexcept;
    UniqueHICON& operator=(UniqueHICON&& other) noexcept;

    operator HICON() const noexcept;

private:
    void Reset() noexcept;

private:
    HICON hicon{};
    bool is_shared_{};
};

}