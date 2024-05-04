#include <zaf/clipboard/medium.h>
#include <zaf/base/error/contract_error.h>
#include <zaf/clipboard/internal/medium_wrapping.h>

namespace zaf::clipboard {

Medium Medium::FromString(std::wstring_view string) {
    return FromGlobalMem(GlobalMem::FromString(string, GlobalMemFlags::Movable));
}


Medium Medium::FromGlobalMem(GlobalMem global_mem) {
    STGMEDIUM inner{};
    inner.tymed = TYMED_HGLOBAL;
    inner.hGlobal = global_mem.Detach();
    return Medium{ inner };
}


Medium::Medium(const STGMEDIUM& inner) : inner_(internal::WrapSTGMEDIUM(inner)) {

}


Medium::~Medium() {
    Reset();
}


Medium::Medium(const Medium& other) : inner_(internal::ShallowCopySTGMEDIUM(other.inner_)) {

}


Medium& Medium::operator=(const Medium& other) {
    if (this != &other) {
        Reset(internal::ShallowCopySTGMEDIUM(other.inner_));
    }
    return *this;
}


Medium::Medium(Medium&& other) : inner_(other.Detach()) {

}


Medium& Medium::operator=(Medium&& other) {
    if (this != &other) {
        Reset(other.Detach());
    }
    return *this;
}


void Medium::Reset(const STGMEDIUM& new_inner) {
    ReleaseStgMedium(&inner_);
    inner_ = internal::WrapSTGMEDIUM(new_inner);
}


STGMEDIUM Medium::Detach() {

    auto result = inner_;
    inner_ = {};
    return result;
}


void Medium::VisitGlobalMem(const std::function<void(const GlobalMem&)>& visitor) const {

    ZAF_EXPECT(Type() == MediumType::GlobalMem);

    GlobalMem global_mem{ inner_.hGlobal };
    try {
        visitor(global_mem);
        auto handle = global_mem.Detach();
    }
    catch (...) {
        auto handle = global_mem.Detach();
        throw;
    }
}


std::wstring Medium::ToString() const {

    std::wstring result;
    VisitGlobalMem([&result](const GlobalMem& global_mem) {
        result = global_mem.ToString();
    });
    return result;
}

}