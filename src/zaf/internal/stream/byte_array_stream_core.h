#pragma once

#include <zaf/base/byte_array.h>
#include <zaf/io/stream/memory_stream_core.h>

namespace zaf::internal {

class ByteArrayStreamCore : public MemoryStreamCore {
public:
    ByteArrayStreamCore() : byte_array_(std::make_shared<ByteArray>()) {

    }

    ByteArrayStreamCore(const void* data, std::size_t size) : 
        byte_array_(std::make_shared<ByteArray>(ByteArray::FromMemory(data, size))) {

    }

    explicit ByteArrayStreamCore(std::size_t initial_size) :
        byte_array_(std::make_shared<ByteArray>()) {

        byte_array_->Resize(initial_size);
    }

    bool CanWrite() noexcept override {
        return true;
    }

    HRESULT GetInfo(std::byte** pointer, std::size_t* size) noexcept override {

        if (pointer) {
            *pointer = byte_array_->Data();
        }

        if (size) {
            *size = byte_array_->Size();
        }

        return S_OK;
    }

    HRESULT Resize(std::size_t new_size) noexcept override {
        try {
            byte_array_->Resize(new_size);
            return S_OK;
        }
        catch (...) {
            return E_OUTOFMEMORY;
        }
    }

    std::unique_ptr<MemoryStreamCore> Clone() noexcept {
        try {
            return std::unique_ptr<ByteArrayStreamCore>(new ByteArrayStreamCore{ byte_array_ });
        }
        catch (...) {
            return nullptr;
        }
    }

private:
    explicit ByteArrayStreamCore(std::shared_ptr<ByteArray> byte_array) :
        byte_array_(std::move(byte_array)) {

    }

private:
    std::shared_ptr<ByteArray> byte_array_;
};

}