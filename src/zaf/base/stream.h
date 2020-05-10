#pragma once

#include <Objidl.h>
#include <cstddef>
#include <cstdint>
#include <zaf/base/com_object.h>
#include <zaf/base/error.h>

namespace zaf {

class Stream : public ComObject<IStream> {
public:
    enum class SeekOrigin {
        Set = STREAM_SEEK_SET,
        Current = STREAM_SEEK_CUR,
        End = STREAM_SEEK_END,
    };

public:
    static Stream FromMemory(const void* data, std::size_t size, std::error_code& error_code);

    static Stream FromMemory(const void* data, std::size_t size) {
        std::error_code error_code;
        auto result = FromMemory(data, size, error_code);
        ZAF_CHECK_ERROR(error_code);
        return result;
    }

public:
    Stream() { }
    Stream(IStream* handle) : ComObject(handle) { }

    IStream* GetHandle() const {
        return static_cast<IStream*>(__super::GetHandle());
    }

    std::int64_t GetLength(std::error_code& error_code) const;

    std::int64_t GetLength() const {
        std::error_code error_code;
        auto result = GetLength(error_code);
        return result;
    }

    std::int64_t Seek(SeekOrigin origin, std::int64_t offset, std::error_code& error_code);

    std::int64_t Seek(SeekOrigin origin, std::int64_t offset) {
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

}