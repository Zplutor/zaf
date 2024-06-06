#pragma once

#include <zaf/base/error/contract_error.h>
#include <zaf/io/stream/memory_stream_core.h>

namespace zaf::internal {

class PlainMemoryStreamCore : public MemoryStreamCore {
public:
    PlainMemoryStreamCore(const void* memory, std::size_t size) :
        memory_(reinterpret_cast<std::byte*>(const_cast<void*>(memory))),
        size_(size),
        can_write_(false) {

        ZAF_EXPECT(memory);
    }

    PlainMemoryStreamCore(void* memory, std::size_t size) :
        memory_(reinterpret_cast<std::byte*>(memory)),
        size_(size),
        can_write_(true) {

        ZAF_EXPECT(memory);
    }

    bool CanWrite() noexcept override {
        return can_write_;
    }

    std::pair<std::byte*, std::size_t> GetInfo() noexcept override {
        return std::make_pair(memory_, size_);
    }

    HRESULT Resize(std::size_t new_size) noexcept override {
        return E_NOTIMPL;
    }

    std::unique_ptr<MemoryStreamCore> Clone() noexcept {
        try {
            if (can_write_) {
                return std::make_unique<PlainMemoryStreamCore>(memory_, size_);
            }
            else {
                return std::make_unique<PlainMemoryStreamCore>(
                    static_cast<const void*>(memory_), size_);
            }
        }
        catch (...) {
            return nullptr;
        }
    }

private:
    std::byte* memory_{};
    std::size_t size_{};
    bool can_write_{};
};

}