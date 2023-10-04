#pragma once

#include <Objidl.h>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <zaf/base/com_object.h>

namespace zaf {

enum class SeekOrigin {
    Begin = STREAM_SEEK_SET,
    Current = STREAM_SEEK_CUR,
    End = STREAM_SEEK_END,
};

class Stream : public COMObject<IStream> {
public:
    /**
    Creates a read-only stream whose data is copied from specified memory.

    @param data
        A memory pointer.

    @param size
        Size of the memory.

    @throw zaf::Error
        Thrown if fail to create the stream.
    */
    static Stream FromMemory(const void* data, std::size_t size);

    /**
    Creates a read-only stream from specified memory without copy.

    @param data
        A memory pointer.

    @param size
        Size of the memory.
    */
    static Stream FromMemoryNoCopy(const void* data, std::size_t size);

    /**
    Creates a read-only stream from file.

    @param path
        A file path.

    @throw zaf::Error
        Thrown if fail to create the stream.
    */
    static Stream FromFile(const std::filesystem::path& path);

public:
    using COMObject::COMObject;

    /**
    Gets the size of the stream.
    */
    std::uint64_t GetSize() const;

    /**
    Gets the position of the stream.
    */
    std::uint64_t GetPosition() const;

    std::uint64_t Seek(SeekOrigin origin, std::int64_t offset);

    std::size_t Read(std::size_t, void* data) const;

    std::size_t Write(const void* data, std::size_t size);

    /**
    Gets a pointer points to the underlying buffer of the stream.

    Currently, only streams created by FromMemoryNoCopy() support the operation.

    @return
        An underlying buffer pointer.

    @throw zaf::Error
        Thrown if the stream doesn't support getting underlying buffer.
    */
    const std::byte* GetUnderlyingBuffer() const;
};

}