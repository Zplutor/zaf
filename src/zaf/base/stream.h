#pragma once

#include <Objidl.h>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <zaf/base/com_object.h>

namespace zaf {

class Stream : public ComObject<IStream> {
public:
    enum class SeekOrigin {
        Set = STREAM_SEEK_SET,
        Current = STREAM_SEEK_CUR,
        End = STREAM_SEEK_END,
    };

public:
    static Stream FromMemory(const void* data, std::size_t size);

    static Stream FromFile(const std::filesystem::path& path);

public:
    Stream() { }
    Stream(IStream* handle) : ComObject(handle) { }

    std::int64_t GetLength() const;

    std::int64_t Seek(SeekOrigin origin, std::int64_t offset);

    std::size_t Read(std::size_t size, void* data) const;

    std::size_t Write(const void* data, std::size_t size);
};

}