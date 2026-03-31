#include <zaf/base/unique_hicon.h>

namespace zaf {

UniqueHICON::UniqueHICON(HICON hicon, bool is_shared) noexcept
    : 
    hicon(hicon),
    is_shared_(is_shared) {

}


UniqueHICON::~UniqueHICON() {
    Reset();
}


UniqueHICON::UniqueHICON(UniqueHICON&& other) noexcept
    : 
    hicon(other.hicon),
    is_shared_(other.is_shared_) {

    other.hicon = nullptr;
    other.is_shared_ = false;
}


UniqueHICON& UniqueHICON::operator=(UniqueHICON&& other) noexcept {

    if (this == &other) {
        return *this;
    }

    Reset();

    hicon = other.hicon;
    is_shared_ = other.is_shared_;

    other.hicon = nullptr;
    other.is_shared_ = false;
    return *this;
}


void UniqueHICON::Reset() noexcept {
    if (hicon && !is_shared_) {
        ::DestroyIcon(hicon);
    }
}


UniqueHICON::operator HICON() const noexcept {
    return hicon;
}

}