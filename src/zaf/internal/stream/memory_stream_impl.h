#pragma once

#include <zaf/base/non_copyable.h>
#include <zaf/io/stream/memory_stream.h>
#include <zaf/io/stream/memory_stream_core.h>

namespace zaf::internal {

class MemoryStreamImpl : public IMemoryStream, NonCopyableNonMovable {
public:
    explicit MemoryStreamImpl(std::unique_ptr<MemoryStreamCore> core);

    MemoryStreamImpl(
        std::unique_ptr<MemoryStreamCore> core,
        std::size_t seek_position);

    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid, void** object) override;
    ULONG STDMETHODCALLTYPE AddRef() override;
    ULONG STDMETHODCALLTYPE Release() override;

    HRESULT STDMETHODCALLTYPE Read(
        void* buffer,
        ULONG expected_read_size,
        ULONG* actual_read_size) override;

    HRESULT STDMETHODCALLTYPE Write(const void* pv, ULONG cb, ULONG* written_size) override;

    HRESULT STDMETHODCALLTYPE Seek(
        LARGE_INTEGER offset,
        DWORD origin,
        ULARGE_INTEGER* new_position) override;

    HRESULT STDMETHODCALLTYPE SetSize(ULARGE_INTEGER libNewSize) override;

    HRESULT STDMETHODCALLTYPE CopyTo(
        IStream* other_stream,
        ULARGE_INTEGER size,
        ULARGE_INTEGER* actual_read_size,
        ULARGE_INTEGER* actual_write_size) override;

    HRESULT STDMETHODCALLTYPE Commit(DWORD grfCommitFlags) override;
    HRESULT STDMETHODCALLTYPE Revert(void) override;

    HRESULT STDMETHODCALLTYPE LockRegion(
        ULARGE_INTEGER libOffset,
        ULARGE_INTEGER cb,
        DWORD dwLockType) override;

    HRESULT STDMETHODCALLTYPE UnlockRegion(
        ULARGE_INTEGER libOffset,
        ULARGE_INTEGER cb,
        DWORD dwLockType) override;

    HRESULT STDMETHODCALLTYPE Stat(STATSTG* state, DWORD grfStatFlag) override;
    HRESULT STDMETHODCALLTYPE Clone(IStream** new_stream) override;

    const std::byte* GetMemory() override;

private:
    ULONG reference_count_{ 1 };
    std::unique_ptr<MemoryStreamCore> core_;
    std::size_t seek_position_{};
};

}