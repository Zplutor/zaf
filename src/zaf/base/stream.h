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
    Creates a stream whose data is copied from the specified memory.

    @param data
        The pointer to the memory.

    @param size
        The size of the memory.

    @pre
        data is not null.

    @throw zaf::PreconditionError
        Thrown if the precondition is violated.

    @throw std::bad_alloc
        Thrown if memory allocation fails.
    */
    static Stream FromMemory(const void* data, std::size_t size);

    /**
    Creates a read-only stream from the specified memory without copy.

    @param data
        The pointer to the memory.

    @param size
        The size of the memory.

    @pre 
        data is not null.

    @throw zaf::PreconditionError
        Thrown if the precondition is violated.

    @throw std::bad_alloc
        Thrown if memory allocation fails.

    @details
        Since the memory is not copied into the stream, users should guarantee that the memory is
        valid during the life cycle of the stream.
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
    std::size_t GetSize() const;

    /**
    Gets the position of the stream.
    */
    std::size_t GetPosition() const;

    std::size_t Seek(SeekOrigin origin, std::int64_t offset);

    std::size_t Read(std::size_t, void* data) const;

    std::size_t Write(const void* data, std::size_t size);

    /**
    Gets a pointer points to the underlying buffer of the stream.

    @return
        An underlying buffer pointer.

    @throw zaf::Error
        Thrown if the stream doesn't support getting underlying buffer.
    */
    const std::byte* GetUnderlyingBuffer() const;
};

}