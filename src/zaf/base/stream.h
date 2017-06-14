#pragma once

#include <Objidl.h>
#include <cstddef>
#include <cstdint>
#include <zaf/base/com_object.h>
#include <zaf/base/error.h>

namespace zaf {

class Stream : public ComObject<IStream> {
public:
    enum class Origin {
        Set = STREAM_SEEK_SET,
        Current = STREAM_SEEK_CUR,
        End = STREAM_SEEK_END,
    };

public:
    Stream() { }
    Stream(IStream* handle) : ComObject(handle) { }

    std::uint64_t GetLength(std::error_code& error_code) const;

    std::uint64_t GetLength() const {
        std::error_code error_code;
        auto result = GetLength(error_code);
        return result;
    }

    std::uint64_t Seek(Origin origin, std::int64_t offset, std::error_code& error_code);

    std::uint64_t Seek(Origin origin, std::int64_t offset) {
        std::error_code error_code;
        auto new_position = Seek(origin, offset, error_code);
        ZAF_CHECK_ERROR(error_code);
        return new_position;
    }

    std::size_t Read(std::size_t size, void* data, std::error_code& error_code) const;

    std::size_t Read(std::size_t size, void* data) const {
        std::error_code error_code;
        auto read_size = Read(size, data, error_code);
        ZAF_CHECK_ERROR(error_code);
        return read_size;
    }

    std::size_t Write(const void* data, std::size_t size, std::error_code& error_code);

    std::size_t Write(const void* data, std::size_t size) {
        std::error_code error_code;
        auto written_size = Write(data, size, error_code);
        ZAF_CHECK_ERROR(error_code);
        return written_size;
    }
};

Stream CreateMemoryStream(const void* initial_data, std::size_t initial_data_size);

inline Stream CreateMemoryStream() {
    return CreateMemoryStream(nullptr, 0);
}

}