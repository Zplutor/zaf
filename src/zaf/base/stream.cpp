#include <zaf/base/stream.h>
#include <Shlwapi.h>
#include <zaf/base/error/com_error.h>

namespace zaf {

Stream Stream::FromMemory(const void* data, std::size_t size) {

    auto handle = SHCreateMemStream(reinterpret_cast<const BYTE*>(data), size);
    if (!handle) {
        ZAF_THROW_IF_COM_ERROR(E_OUTOFMEMORY);
    }

    return Stream{ handle };
}


Stream Stream::FromFile(const std::filesystem::path& path) {

    IStream* handle{};
    HRESULT hresult = SHCreateStreamOnFileEx(
        path.c_str(),
        STGM_READ, 
        FILE_ATTRIBUTE_NORMAL, 
        FALSE, 
        nullptr, 
        &handle);

    ZAF_THROW_IF_COM_ERROR(hresult);
    return Stream{ handle };
}


std::int64_t Stream::GetLength() const {

    STATSTG state = { 0 };
    HRESULT result = GetHandle()->Stat(&state, STATFLAG_NONAME);

    ZAF_THROW_IF_COM_ERROR(result);
    return state.cbSize.QuadPart;
}


std::int64_t Stream::Seek(SeekOrigin origin, std::int64_t offset) {

    LARGE_INTEGER move;
    move.QuadPart = offset;

    ULARGE_INTEGER new_position = { 0 };
    HRESULT result = GetHandle()->Seek(move, static_cast<DWORD>(origin), &new_position);

    ZAF_THROW_IF_COM_ERROR(result);
    return new_position.QuadPart;
}


std::size_t Stream::Read(std::size_t size, void* data) const {

    ULONG read_size = 0;
    HRESULT result = GetHandle()->Read(data, size, &read_size);

    ZAF_THROW_IF_COM_ERROR(result);
    return read_size;
}


std::size_t Stream::Write(const void* data, std::size_t size) {

    ULONG written_size = 0;
    HRESULT result = GetHandle()->Write(data, size, &written_size);

    ZAF_THROW_IF_COM_ERROR(result);
    return written_size;
}


}