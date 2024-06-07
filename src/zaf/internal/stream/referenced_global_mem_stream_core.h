#pragma once

#include <optional>
#include <zaf/base/global_mem.h>
#include <zaf/io/stream/memory_stream_core.h>

namespace zaf::internal {

class ReferencedGlobalMemStreamCore : public MemoryStreamCore {
public:
    explicit ReferencedGlobalMemStreamCore(const GlobalMem& global_mem) :
        global_mem_(const_cast<GlobalMem&>(global_mem)),
        can_write_(false) {

    }

    explicit ReferencedGlobalMemStreamCore(GlobalMem& global_mem) :
        global_mem_(global_mem), 
        can_write_(true) {

    }

    bool CanWrite() noexcept override {
        return can_write_;
    }

    HRESULT GetInfo(std::byte** pointer, std::size_t* size) noexcept override {

        try {
            if (!lock_) {
                lock_.emplace(global_mem_.Lock());
            }

            if (pointer) {
                *pointer = reinterpret_cast<std::byte*>(lock_->Pointer());
            }

            if (size) {
                *size = global_mem_.Size();
            }
            return S_OK;
        }
        catch (...) {
            return E_UNEXPECTED;
        }
    }

    HRESULT Resize(std::size_t new_size) noexcept override {

        if (!can_write_) {
            return E_NOTIMPL;
        }

        try {
            if (lock_) {
                lock_.reset();
            }
            global_mem_.Resize(new_size);
            return S_OK;
        }
        catch (...) {
            return E_OUTOFMEMORY;
        }
    }

    std::unique_ptr<MemoryStreamCore> Clone() noexcept override {
        try {
            if (lock_) {
                lock_.reset();
            }

            if (can_write_) {
                return std::make_unique<ReferencedGlobalMemStreamCore>(global_mem_);
            }
            else {
                return std::make_unique<ReferencedGlobalMemStreamCore>(
                    static_cast<const GlobalMem&>(global_mem_));
            }
        }
        catch (...) {
            return nullptr;
        }
    }

private:
    GlobalMem& global_mem_;
    std::optional<GlobalMemLock> lock_;
    bool can_write_{};
};

}