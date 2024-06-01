#pragma once

#include <Objidl.h>
#include <zaf/base/byte_array.h>
#include <zaf/base/non_copyable.h>

namespace zaf::internal {

constexpr GUID IID_ByteArrayStream = {
    0xd13491c6, 0x344b, 0x4f94, { 0xa9, 0xb3, 0xd5, 0xa3, 0xf4, 0xc4, 0x1b, 0x4e } 
};

class ByteArrayStream : public IStream, NonCopyableNonMovable {
public:
    //Constructs a new ByteArrayStream with initial size.
    ByteArrayStream(std::size_t size) : byte_array_(std::make_shared<ByteArray>()) {
        byte_array_->Resize(size);
    }


    //Constructs a new ByteArrayStream copied from the specified memory.
    ByteArrayStream(const void* data, std::size_t size) : 
        byte_array_(std::make_shared<ByteArray>(ByteArray::FromMemory(data, size))) {

    }


    //Constructs a ByteArrayStream with a shared byte array.
    ByteArrayStream(std::shared_ptr<ByteArray> byte_array, std::size_t seek_position) :
        byte_array_(std::move(byte_array)),
        seek_position_(seek_position) {

    }


    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid, void** object) override {

        if (!object) {
            return E_INVALIDARG;
        }

        if (iid == IID_IUnknown ||
            iid == IID_IStream ||
            iid == IID_ISequentialStream ||
            iid == IID_ByteArrayStream) {

            AddRef();
            *object = reinterpret_cast<void*>(this);
            return S_OK;
        }

        *object = nullptr;
        return E_NOINTERFACE;
    }


    ULONG STDMETHODCALLTYPE AddRef() override {
        return InterlockedIncrement(&reference_count_);
    }


    ULONG STDMETHODCALLTYPE Release() override {

        LONG new_reference_count = InterlockedDecrement(&reference_count_);
        if (new_reference_count == 0) {
            delete this;
        }
        return new_reference_count;
    }


    HRESULT STDMETHODCALLTYPE Read(
        void* buffer,
        ULONG expected_read_size,
        ULONG* actual_read_size) override {

        if (!buffer) {
            return E_INVALIDARG;
        }

        auto [read_pointer, read_size] = GetReadInfo(expected_read_size);

        std::memcpy(buffer, read_pointer, read_size);
        seek_position_ += read_size;

        if (actual_read_size) {
            *actual_read_size = static_cast<ULONG>(read_size);
        }
        return S_OK;
    }


    HRESULT STDMETHODCALLTYPE Write(const void* pv, ULONG cb, ULONG* written_size) override {

        if (!pv) {
            return E_INVALIDARG;
        }

        std::size_t new_size = seek_position_ + cb;
        if (byte_array_->Size() < new_size) {
            try {
                byte_array_->Resize(new_size);
            }
            catch (const std::bad_alloc&) {
                return E_OUTOFMEMORY;
            }
        }

        std::memcpy(&(*byte_array_)[seek_position_], pv, cb);

        if (written_size) {
            *written_size = cb;
        }
        return S_OK;
    }


    HRESULT STDMETHODCALLTYPE Seek(
        LARGE_INTEGER offset,
        DWORD origin,
        ULARGE_INTEGER* new_position) override {

        std::int64_t new_seek_position{};
        if (origin == STREAM_SEEK_SET) {
            new_seek_position = offset.QuadPart;
        }
        else if (origin == STREAM_SEEK_CUR) {
            new_seek_position = seek_position_ + offset.QuadPart;
        }
        else if (origin == STREAM_SEEK_END) {
            new_seek_position = static_cast<std::int64_t>(byte_array_->Size()) + offset.QuadPart;
        }
        else {
            return E_INVALIDARG;
        }

        if (new_seek_position < 0) {
            return E_INVALIDARG;
        }

        if (new_seek_position > static_cast<std::int64_t>(byte_array_->Size())) {
            new_seek_position = byte_array_->Size();
        }

        seek_position_ = new_seek_position;

        if (new_position) {
            new_position->QuadPart = new_seek_position;
        }
        return S_OK;
    }


    HRESULT STDMETHODCALLTYPE SetSize(ULARGE_INTEGER libNewSize) override {
        
        try {
            byte_array_->Resize(libNewSize.QuadPart);
            return S_OK;
        }
        catch (const std::bad_alloc&) {
            return E_OUTOFMEMORY;
        }
    }


    HRESULT STDMETHODCALLTYPE CopyTo(
        IStream* other_stream,
        ULARGE_INTEGER size,
        ULARGE_INTEGER* actual_read_size,
        ULARGE_INTEGER* actual_write_size) override {

        if (!other_stream) {
            return E_INVALIDARG;
        }

        if (static_cast<IStream*>(this) == other_stream) {
            if (actual_read_size) {
                *actual_read_size = size;
            }
            if (actual_write_size) {
                *actual_write_size = size;
            }
            return S_OK;
        }

        auto [read_pointer, read_size] = GetReadInfo(size.QuadPart);

        ULONG write_size{};
        HRESULT result = other_stream->Write(
            read_pointer,
            static_cast<ULONG>(read_size), 
            &write_size);

        if (FAILED(result)) {
            return result;
        }

        if (actual_read_size) {
            actual_read_size->QuadPart = read_size;
        }
        if (actual_write_size) {
            actual_write_size->QuadPart = write_size;
        }
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE Commit(DWORD grfCommitFlags) override {
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE Revert(void) override {
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE LockRegion(
        ULARGE_INTEGER libOffset,
        ULARGE_INTEGER cb,
        DWORD dwLockType) override {

        return E_NOTIMPL;
    }

    HRESULT STDMETHODCALLTYPE UnlockRegion(
        ULARGE_INTEGER libOffset,
        ULARGE_INTEGER cb,
        DWORD dwLockType) override {

        return E_NOTIMPL;
    }

    HRESULT STDMETHODCALLTYPE Stat(STATSTG* state, DWORD grfStatFlag) override {

        if (!state) {
            return E_INVALIDARG;
        }

        state->pwcsName = nullptr;
        state->type = STGTY_LOCKBYTES;
        state->cbSize.QuadPart = byte_array_->Size();
        state->mtime.dwLowDateTime = 0;
        state->mtime.dwHighDateTime = 0;
        state->ctime.dwLowDateTime = 0;
        state->ctime.dwHighDateTime = 0;
        state->atime.dwLowDateTime = 0;
        state->atime.dwHighDateTime = 0;
        state->grfMode = STGM_READWRITE;
        state->grfLocksSupported = 0;
        state->clsid = CLSID_NULL;
        state->grfStateBits = 0;
        state->reserved = 0;
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE Clone(IStream** new_stream) override {

        if (!new_stream) {
            return E_INVALIDARG;
        }

        *new_stream = new ByteArrayStream{ byte_array_, seek_position_ };
        return S_OK;
    }

    const std::byte* Data() const {
        if (byte_array_->IsEmpty()) {
            return nullptr;
        }
        return &(*byte_array_)[0];
    }

private:
    std::pair<const void*, std::size_t> GetReadInfo(std::size_t expected_read_size) const {
        auto read_size = (std::min)(expected_read_size, byte_array_->Size() - seek_position_);
        return std::make_pair(&(*byte_array_)[seek_position_], read_size);
    }

private:
    ULONG reference_count_{ 1 };
    std::shared_ptr<ByteArray> byte_array_;
    std::size_t seek_position_{};
};

}