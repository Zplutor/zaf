#include <zaf/base/stream.h>
#include <Shlwapi.h>
#include <zaf/base/error/com_error.h>
#include <zaf/base/error/invalid_operation_error.h>

namespace zaf {
namespace {

constexpr GUID IID_NotOwnedMemorySteam = { 
    0xf07062dc, 0x6070, 0x45da, { 0xbc, 0x49, 0x48, 0x32, 0xe2, 0x3, 0xfc, 0xa8 } 
};


class NotOwnedMemoryStream : public IStream {
public:
    NotOwnedMemoryStream(
        const BYTE* data, 
        ULONG size,
        ULONG seek_position)
        : 
        reference_count_(1),
        data_(data),
        size_(size),
        seek_position_(seek_position) {

    }

    NotOwnedMemoryStream(const NotOwnedMemoryStream&) = delete;
    NotOwnedMemoryStream& operator=(const NotOwnedMemoryStream&) = delete;

    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid, void** object) override {

        if (!object) {
            return E_INVALIDARG;
        }

        if (iid == IID_IUnknown || 
            iid == IID_IStream || 
            iid == IID_ISequentialStream ||
            iid == IID_NotOwnedMemorySteam) {

            AddRef();
            *object = reinterpret_cast<void*>(this);
            return NOERROR;
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

        ULONG read_size = (std::min)(expected_read_size, size_ - seek_position_);
        std::memcpy(buffer, data_ + seek_position_, read_size);
        seek_position_ += read_size;

        if (actual_read_size) {
            *actual_read_size = read_size;
        }
        return NOERROR;
    }

    HRESULT STDMETHODCALLTYPE Write(const void* pv, ULONG cb, ULONG* written_size) override {
        return E_NOTIMPL;
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
            new_seek_position = size_ + offset.QuadPart;
        }
        else {
            return E_INVALIDARG;
        }

        if (new_seek_position < 0) {
            return E_INVALIDARG;
        }

        if (new_seek_position > static_cast<std::int64_t>(size_)) {
            new_seek_position = size_;
        }

        seek_position_ = static_cast<ULONG>(new_seek_position);

        if (new_position) {
            new_position->QuadPart = new_seek_position;
        }
        return NOERROR;
    }

    HRESULT STDMETHODCALLTYPE SetSize(ULARGE_INTEGER libNewSize) override {
        return E_NOTIMPL;
    }

    HRESULT STDMETHODCALLTYPE CopyTo(
        IStream* other_stream,
        ULARGE_INTEGER size,
        ULARGE_INTEGER* actual_read_size,
        ULARGE_INTEGER* actual_write_size) override {

        auto buffer = std::make_unique<BYTE[]>(static_cast<std::size_t>(size.QuadPart));

        ULONG read_size{};
        HRESULT result = this->Read(buffer.get(), static_cast<ULONG>(size.QuadPart), &read_size);
        if (FAILED(result)) {
            return result;
        }

        ULONG write_size{};
        result = other_stream->Write(buffer.get(), read_size, &write_size);
        if (FAILED(result)) {
            return result;
        }

        if (actual_read_size) {
            actual_read_size->QuadPart = read_size;
        }
        if (actual_write_size) {
            actual_write_size->QuadPart = write_size;
        }
        return NOERROR;
    }

    HRESULT STDMETHODCALLTYPE Commit(DWORD grfCommitFlags) override {
        return E_NOTIMPL;
    }

    HRESULT STDMETHODCALLTYPE Revert(void) override {
        return E_NOTIMPL;
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
        state->cbSize.QuadPart = size_;
        state->mtime.dwLowDateTime = 0;
        state->mtime.dwHighDateTime = 0;
        state->ctime.dwLowDateTime = 0;
        state->ctime.dwHighDateTime = 0;
        state->atime.dwLowDateTime = 0;
        state->atime.dwHighDateTime = 0;
        state->grfMode = STGM_READ;
        state->grfLocksSupported = 0;
        state->clsid = CLSID_NULL;
        state->grfStateBits = 0;
        state->reserved = 0;
        return NOERROR;
    }

    HRESULT STDMETHODCALLTYPE Clone(IStream** new_stream) override {

        if (!new_stream) {
            return E_INVALIDARG;
        }

        *new_stream = new NotOwnedMemoryStream{ data_, size_, seek_position_ };
        return NOERROR;
    }

    const std::byte* Data() const {
        return reinterpret_cast<const std::byte*>(data_);
    }

private:
    LONG reference_count_{};

    const BYTE* data_{};
    ULONG size_{};
    ULONG seek_position_{};
};

}

Stream Stream::FromMemory(const void* data, std::size_t size) {

    auto ptr = SHCreateMemStream(reinterpret_cast<const BYTE*>(data), static_cast<UINT>(size));
    if (!ptr) {
        ZAF_THROW_IF_COM_ERROR(E_OUTOFMEMORY);
    }

    return Stream{ ToCOMPtr(ptr) };
}


Stream Stream::FromMemoryNoCopy(const void* data, std::size_t size) {

    return Stream{ 
        MakeCOMPtr<NotOwnedMemoryStream>(
            reinterpret_cast<const BYTE*>(data),
            static_cast<ULONG>(size),
            0)
    };
}


Stream Stream::FromFile(const std::filesystem::path& path) {

    COMPtr<IStream> inner;
    HRESULT hresult = SHCreateStreamOnFileEx(
        path.c_str(),
        STGM_READ, 
        FILE_ATTRIBUTE_NORMAL, 
        FALSE, 
        nullptr, 
        inner.Reset());

    ZAF_THROW_IF_COM_ERROR(hresult);
    return Stream{ inner };
}


std::uint64_t Stream::GetSize() const {

    STATSTG state{};
    HRESULT result = Inner()->Stat(&state, STATFLAG_NONAME);

    ZAF_THROW_IF_COM_ERROR(result);
    return state.cbSize.QuadPart;
}


std::uint64_t Stream::GetPosition() const {
    return const_cast<Stream*>(this)->Seek(SeekOrigin::Current, 0);
}


std::uint64_t Stream::Seek(SeekOrigin origin, std::int64_t offset) {

    LARGE_INTEGER move;
    move.QuadPart = offset;

    ULARGE_INTEGER new_position{};
    HRESULT result = Inner()->Seek(move, static_cast<DWORD>(origin), &new_position);

    ZAF_THROW_IF_COM_ERROR(result);
    return new_position.QuadPart;
}


std::uint64_t Stream::Read(std::uint64_t size, void* data) const {

    ULONG read_size = 0;
    HRESULT result = Inner()->Read(data, static_cast<ULONG>(size), &read_size);

    ZAF_THROW_IF_COM_ERROR(result);
    return read_size;
}


std::size_t Stream::Write(const void* data, std::size_t size) {

    ULONG written_size = 0;
    HRESULT result = Inner()->Write(data, static_cast<ULONG>(size), &written_size);

    ZAF_THROW_IF_COM_ERROR(result);
    return written_size;
}


const std::byte* Stream::GetUnderlyingBuffer() const {

    auto memory_stream = Inner().Query<NotOwnedMemoryStream>(IID_NotOwnedMemorySteam);
    if (memory_stream) {
        return memory_stream->Data();
    }

    throw InvalidOperationError{ ZAF_SOURCE_SITE() };
}


}