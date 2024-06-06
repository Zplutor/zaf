#pragma once

#include <Objidl.h>
#include <zaf/base/non_copyable.h>

namespace zaf {

class MemoryStreamCore : NonCopyableNonMovable {
public:
    MemoryStreamCore() noexcept = default;
    virtual ~MemoryStreamCore() = default;

    virtual bool CanWrite() noexcept = 0;
    virtual std::pair<std::byte*, std::size_t> GetInfo() noexcept = 0;
    virtual HRESULT Resize(std::size_t new_size) noexcept = 0;
    virtual std::unique_ptr<MemoryStreamCore> Clone() noexcept = 0;
};

}