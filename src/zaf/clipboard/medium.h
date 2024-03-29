#pragma once

#include <objidl.h>
#include <filesystem>
#include <functional>
#include <string_view>
#include <zaf/base/global_mem.h>
#include <zaf/base/non_copyable.h>
#include <zaf/clipboard/medium_type.h>

namespace zaf::clipboard {

class Medium {
public:
    /**
    Creates a medium from specified string, using movable global memory as storage.
    */
    static Medium FromString(std::wstring_view string);
    static Medium FromGlobalMem(GlobalMem global_mem);

public:
    Medium() = default;
    explicit Medium(const STGMEDIUM& inner);

    ~Medium();

    Medium(const Medium& other);
    Medium& operator=(const Medium& other);

    Medium(Medium&& other);
    Medium& operator=(Medium&& other);

    MediumType Type() const {
        return static_cast<MediumType>(inner_.tymed);
    }

    void Reset(const STGMEDIUM& new_inner = {});

    [[nodiscard]]
    STGMEDIUM Detach();

    const STGMEDIUM& Inner() const {
        return inner_;
    }

    void VisitGlobalMem(const std::function<void(const GlobalMem&)>& visitor) const;

    /**
    Gets string from global memory.

    @throw zaf::Error
        Fails to get string from global memory.

    @throw std::logic_error
        Medium type is not global memory.
    */
    std::wstring ToString() const;

private:
    STGMEDIUM inner_{};
};

}