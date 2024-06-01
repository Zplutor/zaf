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
    Creates a stream from the memory with the specified initial size.

    @param initial_size
        The initial size of the stream.

    @return
        The new stream. The content of the stream is zero-initialized.

    @post
        The returned stream is not null.

    @throw std::bad_alloc
        Thrown if memory allocation fails.
    */
    static Stream FromMemory(std::size_t initial_size);

    /**
    Creates a stream whose data is copied from the specified memory.

    @param data
        The pointer to the memory.

    @param size
        The size of the memory.

    @pre
        data is not null.

    @return
        The new stream.

    @post
        The returned stream is not null.

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

    @return
        The size of the stream.

    @throw zaf::COMError
        Thrown if the operation fails.
    */
    std::size_t GetSize() const;

    /**
    Gets the current read write position of the stream.

    @return
        The current read write position of the stream.

    @throw zaf::COMError
        Thrown if the operation fails.
    */
    std::size_t GetPosition() const;

    std::size_t Seek(SeekOrigin origin, std::int64_t offset);

    /**
    Reads content at the current position from the stream.

    @param size
        The content size to read, in bytes.

    @param data
        A pointer to the buffer that will receive the read content.

    @pre
        data is not null.

    @return
        The size of the content that is written to the buffer, in bytes.

    @throw zaf::COMError
        Thrown if the operation fails.
    */
    std::size_t Read(std::size_t size, void* data) const;

    /**
    Writes content to the stream at the current position.

    @param data
        A pointer to the buffer from which the content will be read.

    @param size
        The size of the content what will be written, in bytes.

    @pre 
        data is not null.

    @return
        The size of the content that is written to the stream, in bytes.

    @throw zaf::COMError
        Thrown if the operation fails.
    */
    std::size_t Write(const void* data, std::size_t size);

    /**
    Gets the pointer to the underlying buffer that can access all content of the stream.

    @return
        The underlying buffer pointer. May be null if the stream doesn't have such a buffer.

    @note
        Only streams created from memory support getting the underlying buffer. The result may be 
        null if the size of the stream is zero.
    */
    const std::byte* GetUnderlyingBuffer() const noexcept;
};

}