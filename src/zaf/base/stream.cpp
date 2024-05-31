#include <zaf/base/stream.h>
#include <Shlwapi.h>
#include <zaf/base/error/com_error.h>
#include <zaf/base/error/invalid_operation_error.h>
#include <zaf/internal/byte_array_stream.h>
#include <zaf/internal/no_copy_memory_stream.h>

namespace zaf {

Stream Stream::FromMemory(const void* data, std::size_t size) {

    auto ptr = new internal::ByteArrayStream{ data, size };
    return Stream{ ToCOMPtr(ptr) };
}


Stream Stream::FromMemoryNoCopy(const void* data, std::size_t size) {

    return Stream{ 
        MakeCOMPtr<internal::NoCopyMemoryStream>(
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


std::size_t Stream::GetSize() const {

    STATSTG state{};
    HRESULT result = Ptr()->Stat(&state, STATFLAG_NONAME);

    ZAF_THROW_IF_COM_ERROR(result);
    return state.cbSize.QuadPart;
}


std::size_t Stream::GetPosition() const {
    return const_cast<Stream*>(this)->Seek(SeekOrigin::Current, 0);
}


std::size_t Stream::Seek(SeekOrigin origin, std::int64_t offset) {

    LARGE_INTEGER move;
    move.QuadPart = offset;

    ULARGE_INTEGER new_position{};
    HRESULT result = Ptr()->Seek(move, static_cast<DWORD>(origin), &new_position);

    ZAF_THROW_IF_COM_ERROR(result);
    return new_position.QuadPart;
}


std::size_t Stream::Read(std::uint64_t size, void* data) const {

    ULONG read_size = 0;
    HRESULT result = Ptr()->Read(data, static_cast<ULONG>(size), &read_size);

    ZAF_THROW_IF_COM_ERROR(result);
    return read_size;
}


std::size_t Stream::Write(const void* data, std::size_t size) {

    ULONG written_size = 0;
    HRESULT result = Ptr()->Write(data, static_cast<ULONG>(size), &written_size);

    ZAF_THROW_IF_COM_ERROR(result);
    return written_size;
}


const std::byte* Stream::GetUnderlyingBuffer() const {

    auto no_copy_stream = Ptr().Query<internal::NoCopyMemoryStream>(
        internal::IID_NoCopyMemoryStream);

    if (no_copy_stream) {
        return no_copy_stream->Data();
    }

    auto byte_array_stream = Ptr().Query<internal::ByteArrayStream>(internal::IID_ByteArrayStream);
    if (byte_array_stream) {
        return byte_array_stream->Data();
    }

    throw InvalidOperationError{ ZAF_SOURCE_LOCATION() };
}


}