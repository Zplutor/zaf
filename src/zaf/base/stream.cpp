#include <zaf/base/stream.h>
#include <Shlwapi.h>

namespace zaf {

Stream Stream::FromMemory(const void* data, std::size_t size, std::error_code& error_code) {

    auto handle = SHCreateMemStream(reinterpret_cast<const BYTE*>(data), size);
    if (!handle) {
        error_code = MakeComErrorCode(E_OUTOFMEMORY);
        return {};
    }
    error_code.clear();
    return Stream{ handle };
}


Stream Stream::FromFile(const std::filesystem::path& path, std::error_code& error_code) {

    IStream* handle{};
    HRESULT hresult = SHCreateStreamOnFileEx(
        path.c_str(),
        STGM_READ, 
        FILE_ATTRIBUTE_NORMAL, 
        FALSE, 
        nullptr, 
        &handle);

    error_code = MakeComErrorCode(hresult);
    if (!IsSucceeded(error_code)) {
        return {};
    }

    return Stream{ handle };
}


std::int64_t Stream::GetLength(std::error_code& error_code) const {

    STATSTG state = { 0 };
    HRESULT result = GetHandle()->Stat(&state, STATFLAG_NONAME);

    error_code = MakeComErrorCode(result);
    return state.cbSize.QuadPart;
}


std::int64_t Stream::Seek(SeekOrigin origin, std::int64_t offset, std::error_code& error_code) {

    LARGE_INTEGER move;
    move.QuadPart = offset;

    ULARGE_INTEGER new_position = { 0 };
    HRESULT result = GetHandle()->Seek(move, static_cast<DWORD>(origin), &new_position);

    error_code = MakeComErrorCode(result);
    return new_position.QuadPart;
}


std::size_t Stream::Read(std::size_t size, void* data, std::error_code& error_code) const {

    ULONG read_size = 0;
    HRESULT result = GetHandle()->Read(data, size, &read_size);

    error_code = MakeComErrorCode(result);
    return read_size;
}


std::size_t Stream::Write(const void* data, std::size_t size, std::error_code& error_code) {

    ULONG written_size = 0;
    HRESULT result = GetHandle()->Write(data, size, &written_size);

    error_code = MakeComErrorCode(result);
    return written_size;
}


}