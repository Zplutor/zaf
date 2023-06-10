#include <zaf/clipboard/medium.h>
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


bool Medium::VisitGlobalMem(const std::function<void(const GlobalMem&)>& visitor) const {

    if (Type() != MediumType::GlobalMem) {
        return false;
    }

    GlobalMem global_mem{ inner_.hGlobal };
    visitor(global_mem);
    auto handle = global_mem.Detach();
    return true;
}


bool Medium::VisitFile(const std::function<void(const std::filesystem::path&)>& visitor) const {
    return false;
}

}