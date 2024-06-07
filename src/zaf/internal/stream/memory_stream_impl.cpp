#include <zaf/internal/stream/memory_stream_impl.h>

namespace zaf::internal {

MemoryStreamImpl::MemoryStreamImpl(std::unique_ptr<MemoryStreamCore> core) : 
    core_(std::move(core)) {

}


MemoryStreamImpl::MemoryStreamImpl(
    std::unique_ptr<MemoryStreamCore> core,
    std::size_t seek_position)
    :
    core_(std::move(core)),
    seek_position_(seek_position) {

}


HRESULT __stdcall MemoryStreamImpl::QueryInterface(REFIID iid, void** object) {

    if (!object) {
        return E_INVALIDARG;
    }

    if (iid == IID_IUnknown ||
        iid == IID_ISequentialStream ||
        iid == IID_IStream ||
        iid == IID_IMemoryStream) {

        *object = this;
        AddRef();
        return S_OK;
    }

    *object = nullptr;
    return E_NOINTERFACE;
}


ULONG __stdcall MemoryStreamImpl::AddRef() {
    return InterlockedIncrement(&reference_count_);
}


ULONG __stdcall MemoryStreamImpl::Release() {
    LONG new_reference_count = InterlockedDecrement(&reference_count_);
    if (new_reference_count == 0) {
        delete this;
    }
    return new_reference_count;
}


HRESULT __stdcall MemoryStreamImpl::Read(
    void* buffer,
    ULONG expected_read_size,
    ULONG* actual_read_size) {

    if (!buffer) {
        return STG_E_INVALIDPOINTER;
    }

    auto [pointer, size] = core_->GetInfo();

    auto read_size =
        (std::min)(static_cast<std::size_t>(expected_read_size), size - seek_position_);

    std::memcpy(buffer, pointer + seek_position_, read_size);
    seek_position_ += read_size;

    if (actual_read_size) {
        *actual_read_size = static_cast<ULONG>(read_size);
    }
    return S_OK;
}


HRESULT __stdcall MemoryStreamImpl::Write(const void* pv, ULONG cb, ULONG* written_size) {

    if (!core_->CanWrite()) {
        return E_NOTIMPL;
    }

    if (!pv) {
        return STG_E_INVALIDPOINTER;
    }

    auto [pointer, size] = core_->GetInfo();
    ULONG write_size = cb;

    std::size_t new_size = seek_position_ + write_size;
    if (size < new_size) {
        HRESULT resize_result = core_->Resize(new_size);
        if (FAILED(resize_result)) {
            write_size = static_cast<ULONG>(size - seek_position_);
            if (write_size == 0) {
                return STG_E_MEDIUMFULL;
            }
        }
        else {
            auto new_info = core_->GetInfo();
            pointer = new_info.first;
            size = new_info.second;
        }
    }

    std::memcpy(pointer + seek_position_, pv, write_size);
    seek_position_ += write_size;

    if (written_size) {
        *written_size = write_size;
    }
    return S_OK;
}


HRESULT __stdcall MemoryStreamImpl::Seek(
    LARGE_INTEGER offset, 
    DWORD origin, 
    ULARGE_INTEGER* new_position) {

    auto [pointer, size] = core_->GetInfo();

    std::int64_t new_seek_position{};
    if (origin == STREAM_SEEK_SET) {
        new_seek_position = offset.QuadPart;
    }
    else if (origin == STREAM_SEEK_CUR) {
        new_seek_position = seek_position_ + offset.QuadPart;
    }
    else if (origin == STREAM_SEEK_END) {
        new_seek_position = static_cast<std::int64_t>(size) + offset.QuadPart;
    }
    else {
        return E_INVALIDARG;
    }

    if (new_seek_position < 0) {
        return E_INVALIDARG;
    }

    if (new_seek_position > static_cast<std::int64_t>(size)) {
        new_seek_position = size;
    }

    seek_position_ = new_seek_position;

    if (new_position) {
        new_position->QuadPart = new_seek_position;
    }
    return S_OK;
}


HRESULT __stdcall MemoryStreamImpl::SetSize(ULARGE_INTEGER libNewSize) {
    return core_->Resize(libNewSize.QuadPart);
}


HRESULT __stdcall MemoryStreamImpl::CopyTo(
    IStream* other_stream,
    ULARGE_INTEGER size, 
    ULARGE_INTEGER* actual_read_size,
    ULARGE_INTEGER* actual_write_size) {
    
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

    auto [read_pointer, read_size] = core_->GetInfo();

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


HRESULT __stdcall MemoryStreamImpl::Commit(DWORD grfCommitFlags) {
    return S_OK;
}


HRESULT __stdcall MemoryStreamImpl::Revert(void) {
    return S_OK;
}


HRESULT __stdcall MemoryStreamImpl::LockRegion(
    ULARGE_INTEGER libOffset,
    ULARGE_INTEGER cb, 
    DWORD dwLockType) {

    return E_NOTIMPL;
}


HRESULT __stdcall MemoryStreamImpl::UnlockRegion(
    ULARGE_INTEGER libOffset, 
    ULARGE_INTEGER cb,
    DWORD dwLockType) {

    return E_NOTIMPL;
}


HRESULT __stdcall MemoryStreamImpl::Stat(STATSTG* state, DWORD grfStatFlag) {

    if (!state) {
        return E_INVALIDARG;
    }

    auto [pointer, size] = core_->GetInfo();

    state->pwcsName = nullptr;
    state->type = STGTY_LOCKBYTES;
    state->cbSize.QuadPart = size;
    state->mtime.dwLowDateTime = 0;
    state->mtime.dwHighDateTime = 0;
    state->ctime.dwLowDateTime = 0;
    state->ctime.dwHighDateTime = 0;
    state->atime.dwLowDateTime = 0;
    state->atime.dwHighDateTime = 0;
    state->grfMode = core_->CanWrite() ? STGM_READWRITE : STGM_READ;
    state->grfLocksSupported = 0;
    state->clsid = CLSID_NULL;
    state->grfStateBits = 0;
    state->reserved = 0;
    return S_OK;
}


HRESULT __stdcall MemoryStreamImpl::Clone(IStream** new_stream) {

    if (!new_stream) {
        return E_INVALIDARG;
    }

    auto new_core = core_->Clone();
    if (!new_core) {
        return E_OUTOFMEMORY;
    }

    try {
        *new_stream = new MemoryStreamImpl{ std::move(new_core) , seek_position_ };
        return S_OK;
    }
    catch (const std::bad_alloc&) {
        return E_OUTOFMEMORY;
    }
}


const std::byte* MemoryStreamImpl::GetMemory() {
    return core_->GetInfo().first;
}

}