#pragma once

#include <objidl.h>
#include <filesystem>
#include <functional>
#include <zaf/base/clipboard/data_medium_type.h>
#include <zaf/base/global_mem.h>
#include <zaf/base/non_copyable.h>

namespace zaf {

class DataMedium : NonCopyable {
public:
    static DataMedium FromGlobalMem(GlobalMem global_mem) {
        STGMEDIUM inner{};
        inner.tymed = static_cast<DWORD>(DataMediumType::GlobalMem);
        inner.hGlobal = global_mem.Detach();
        return DataMedium{ inner };
    }

public:
    DataMedium() = default;
    explicit DataMedium(const STGMEDIUM& inner) : inner_(inner) {

    }

    ~DataMedium() {
        ReleaseStgMedium(&inner_);
    }

    DataMedium(DataMedium&& other) : inner_(other.Detach()) {

    }

    DataMedium& operator=(DataMedium&& other) {
        if (this != &other) {
            ReleaseStgMedium(&inner_);
            inner_ = other.Detach();
        }
        return *this;
    }

    DataMediumType Type() const {
        return static_cast<DataMediumType>(inner_.tymed);
    }

    bool VisitGlobalMem(const std::function<void(const GlobalMem&)>& visitor) const {

        if (Type() != DataMediumType::GlobalMem) {
            return false;
        }

        GlobalMem global_mem{ inner_.hGlobal };
        visitor(global_mem);
        auto handle = global_mem.Detach();
        return true;
    }

    bool VisitFile(const std::function<void(const std::filesystem::path&)>& visitor) const {
        return false;
    }

    [[nodiscard]]
    STGMEDIUM Detach() {
        auto result = inner_;
        inner_ = {};
        return result;
    }

    const STGMEDIUM& Inner() const {
        return inner_;
    }

private:
    STGMEDIUM inner_{};
};

}