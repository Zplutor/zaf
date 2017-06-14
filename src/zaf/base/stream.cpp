#include <zaf/base/stream.h>
#include <Shlwapi.h>

namespace zaf {

std::uint64_t Stream::GetLength(std::error_code& error_code) const {

    STATSTG state = { 0 };
    HRESULT result = GetHandle()->Stat(&state, STATFLAG_NONAME);

    error_code = MakeComErrorCode(result);
    return state.cbSize.QuadPart;
}


std::uint64_t Stream::Seek(Origin origin, std::int64_t offset, std::error_code& error_code) {

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


Stream CreateMemoryStream(const void* initial_data, std::size_t initial_data_size) {

    auto handle = SHCreateMemStream(reinterpret_cast<const BYTE*>(initial_data), initial_data_size);
    return Stream(handle);
}


}