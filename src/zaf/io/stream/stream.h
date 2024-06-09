#pragma once

/**
@file
    Defines the class zaf::Stream.
*/

#include <Objidl.h>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <zaf/base/com_object.h>

namespace zaf {

class GlobalMem;

/**
The origin from which the offset is applied when seeking within a stream.
*/
enum class SeekOrigin {

    /**
    Seeks relative to the beginning of the stream.
    */
    Begin = STREAM_SEEK_SET,

    /**
    Seeks relative to the current position in the stream.
    */
    Current = STREAM_SEEK_CUR,

    /**
    Seeks relative to the end of the stream.
    */
    End = STREAM_SEEK_END,
};


/**
An encapsulation class for the IStream interface.
*/
class Stream : public COMObject<IStream> {
public:
    /**
    Creates a memory stream with the specified initial size.

    @param initial_size
        The initial size of the stream.

    @return
        The new memory stream. The content of the stream is zero-initialized.

    @post
        The returned stream is not null.

    @throw std::bad_alloc
        Thrown if memory allocation fails.
    */
    static Stream FromMemory(std::size_t initial_size);

    /**
    Creates a memory stream whose data is copied from the specified memory.

    @param data
        The pointer to the memory.

    @param size
        The size of the memory.

    @pre
        data is not null.

    @return
        The new memory stream.

    @post
        The returned stream is not null.

    @throw zaf::PreconditionError
        Thrown if the precondition is violated.

    @throw std::bad_alloc
        Thrown if memory allocation fails.
    */
    static Stream FromMemory(const void* data, std::size_t size);

    /**
    Creates a read-only stream on the specified memory.

    @param data
        The pointer to the memory. The memory won't be copied into the stream, users should ensure 
        that the memory is valid during the lifecycle of the stream.

    @param size
        The size of the memory.

    @pre 
        data is not null.

    @return
        The stream that reads directly from the specified memory.

    @post
        The returned stream is not null.

    @throw zaf::PreconditionError
        Thrown if the precondition is violated.

    @throw std::bad_alloc
        Thrown if fails to create the stream.
    */
    static Stream CreateOnMemory(const void* data, std::size_t size);

    /**
    Creates a read-write stream on the specified memory.

    @param data
        The pointer to the memory. The memory won't be copied into the stream, users should ensure 
        that the memory is valid during the lifecycle of the stream.

    @param size
        The size of the memory.

    @pre
        data is not null.

    @return
        The stream that reads from and writes to the specified memory directly. The length of the
        stream is fixed.

    @post
        The returned stream is not null.

    @throw zaf::PreconditionError
        Thrown if the precondition is violated.

    @throw std::bad_alloc
        Thrown if fails to create the stream.
    */
    static Stream CreateOnMemory(void* data, std::size_t size);

    /**
    Creates a read-only stream on the specified GlobalMem object.

    @param global_mem
        The GlobalMem object on which the stream created. The content of the object won't be copied
        into the stream, users should ensure that the object is valid during the lifecycle of the
        stream.

    @pre
        global_mem is not null.

    @return
        The stream that reads directly from the specified GlobalMem object.

    @post
        The returned stream is not null.

    @throw zaf::PreconditionError
        Thrown if the precondition is violated.

    @throw std::bad_alloc
        Thrown if fails to create the stream.
    */
    static Stream CreateOnGlobalMem(const GlobalMem& global_mem);

    /**
    Creates a read-write stream on the specified GlobalMem object.

    @param global_mem
        The GlobalMem object on which the stream created. The content of the object won't be copied
        into the stream, users should ensure that the object is valid during the lifecycle of the 
        stream.

    @pre
        global_mem is not null.

    @return
        The stream that reads from and writes to the specified GlobalMem object directly.

    @post
        The returned stream is not null.

    @throw zaf::PreconditionError
        Thrown if the precondition is violated.

    @throw std::bad_alloc
        Thrown if fails to create the stream.
    */
    static Stream CreateOnGlobalMem(GlobalMem& global_mem);

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
    std::size_t Size() const;

    /**
    Gets the current read write position of the stream.

    @return
        The current read write position of the stream.

    @throw zaf::COMError
        Thrown if the operation fails.
    */
    std::size_t Position() const;

    /**
    Indicates whether the stream is writable.

    @return
        True if the stream is writable; otherwise false.

    @throw zaf::COMError
        Thrown if the operation fails.
    */
    bool CanWrite() const;

    /**
    Changes the read write position of the stream.

    @param origin
        The origin from which the offset is applied.
 
    @param offset
        The offset, in bytes, to move the position of the stream. A positive value moves the 
        position forward, while a negative value moves it backward.
 
    @return
        The new position of the stream, measured in bytes from the beginning.

    @throw zaf::COMError
        Thrown if fails to change the position.
    */
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
        The pointer to the underlying buffer. May be null if the stream doesn't have such a buffer,
        or the size of the stream is zero.

    @note
        Only memory streams support getting the underlying buffer.
    */
    const std::byte* UnderlyingBuffer() const noexcept;
};

}