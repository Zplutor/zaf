#include <zaf/io/stream/stream.h>
#include <Shlwapi.h>
#include <zaf/base/error/com_error.h>
#include <zaf/base/error/contract_error.h>
#include <zaf/base/error/invalid_operation_error.h>
#include <zaf/internal/stream/byte_array_stream_core.h>
#include <zaf/internal/stream/memory_stream_impl.h>
#include <zaf/internal/stream/plain_memory_core.h>

namespace zaf {

Stream Stream::FromMemory(std::size_t initial_size) {

    auto core = std::make_unique<internal::ByteArrayStreamCore>(initial_size);
    return Stream{ MakeCOMPtr<internal::MemoryStreamImpl>(std::move(core)) };
}


Stream Stream::FromMemory(const void* data, std::size_t size) {

    auto core = std::make_unique<internal::ByteArrayStreamCore>(data, size);
    return Stream{ MakeCOMPtr<internal::MemoryStreamImpl>(std::move(core)) };
}


Stream Stream::CreateOnMemory(const void* data, std::size_t size) {

    auto core = std::make_unique<internal::PlainMemoryStreamCore>(data, size);
    return Stream{ MakeCOMPtr<internal::MemoryStreamImpl>(std::move(core)) };
}


Stream Stream::CreateOnMemory(void* data, std::size_t size) {

    auto core = std::make_unique<internal::PlainMemoryStreamCore>(data, size);
    return Stream{ MakeCOMPtr<internal::MemoryStreamImpl>(std::move(core)) };
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


std::size_t Stream::GetSize() const {

    STATSTG state{};
    HRESULT result = Ptr()->Stat(&state, STATFLAG_NONAME);
    ZAF_THROW_IF_COM_ERROR(result);
    return state.cbSize.QuadPart;
}


std::size_t Stream::GetPosition() const {
    return const_cast<Stream*>(this)->Seek(SeekOrigin::Current, 0);
}


bool Stream::CanWrite() const {

    STATSTG state{};
    HRESULT result = Ptr()->Stat(&state, STATFLAG_NONAME);
    ZAF_THROW_IF_COM_ERROR(result);

    return (state.grfMode == STGM_WRITE) || (state.grfMode == STGM_READWRITE);
}


std::size_t Stream::Seek(SeekOrigin origin, std::int64_t offset) {

    LARGE_INTEGER move;
    move.QuadPart = offset;

    ULARGE_INTEGER new_position{};
    HRESULT result = Ptr()->Seek(move, static_cast<DWORD>(origin), &new_position);

    ZAF_THROW_IF_COM_ERROR(result);
    return new_position.QuadPart;
}


std::size_t Stream::Read(std::size_t size, void* data) const {

    ZAF_EXPECT(data);

    ULONG read_size = 0;
    HRESULT result = Ptr()->Read(data, static_cast<ULONG>(size), &read_size);

    ZAF_THROW_IF_COM_ERROR(result);
    return read_size;
}


std::size_t Stream::Write(const void* data, std::size_t size) {

    ZAF_EXPECT(data);

    ULONG written_size = 0;
    HRESULT result = Ptr()->Write(data, static_cast<ULONG>(size), &written_size);

    ZAF_THROW_IF_COM_ERROR(result);
    return written_size;
}


const std::byte* Stream::GetUnderlyingBuffer() const noexcept {

    auto memory_stream = Ptr().Query<IMemoryStream>(IID_IMemoryStream);
    if (memory_stream) {
        return memory_stream->GetMemory();
    }
    return nullptr;
}

}