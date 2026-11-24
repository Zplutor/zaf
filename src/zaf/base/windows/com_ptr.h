#pragma once

#include <unknwn.h>
#include <type_traits>

namespace zaf {

template<typename T>
class COMPtr {
public:
    static_assert(std::is_base_of_v<IUnknown, T>);

public:
    COMPtr() noexcept : inner_(nullptr) { 

    }

    COMPtr(std::nullptr_t) noexcept : inner_(nullptr) {

    }

    explicit COMPtr(T* inner) noexcept : inner_(inner) {

    }

    virtual ~COMPtr() {
        Reset();
    }

    COMPtr(const COMPtr& other) noexcept {
        CopyFrom(other.Inner());
    }

    template<typename K, std::enable_if_t<std::is_base_of_v<T, K>, int> = 0>
    COMPtr(const COMPtr<K>& other) noexcept {
        CopyFrom(other.Inner());
    }

    COMPtr(COMPtr&& other) noexcept : inner_(other.Detach()) {

    }

    template<typename K, std::enable_if_t<std::is_base_of_v<T, K>, int> = 0>
    COMPtr(COMPtr<K>&& other) noexcept : inner_(other.Detach()) {

    }

    COMPtr& operator=(const COMPtr& other) noexcept {
        if (this != &other) {
            Reset();
            CopyFrom(other.Inner());
        }
        return *this;
    }

    template<typename K, std::enable_if_t<std::is_base_of_v<T, K>, int> = 0>
    COMPtr& operator=(const COMPtr<K>& other) noexcept {
        Reset();
        CopyFrom(other.Inner());
        return *this;
    }

    COMPtr& operator=(COMPtr&& other) noexcept {
        if (this != &other) {
            Reset();
            inner_ = other.Detach();
        }
        return *this;
    }

    template<typename K, std::enable_if_t<std::is_base_of_v<T, K>, int> = 0>
    COMPtr& operator=(COMPtr<K>&& other) noexcept {
        Reset();
        inner_ = other.Detach();
        return *this;
    }

    bool operator==(std::nullptr_t null) const noexcept {
        return !IsValid();
    }

    bool operator!=(std::nullptr_t null) const noexcept {
        return IsValid();
    }

    bool operator==(const COMPtr& other) const noexcept {
        return inner_ == other.inner_;
    }

    bool operator!=(const COMPtr& other) const noexcept {
        return inner_ != other.inner_;
    }

    explicit operator bool() const noexcept {
        return IsValid();
    }

    T* operator->() const noexcept {
        return inner_;
    }

    T** Reset() noexcept {
        Reset(nullptr);
        return &inner_;
    }

    void** ResetAsVoid() noexcept {
        return reinterpret_cast<void**>(Reset());
    }

    void Reset(T* new_inner) noexcept {
        if (inner_ != new_inner) {
            if (inner_) {
                inner_->Release();
            }
            inner_ = new_inner;
        }
    }

    T* Detach() noexcept {
        auto result = inner_;
        inner_ = nullptr;
        return result;
    }

    bool IsValid() const noexcept {
        return !!inner_;
    }

    template<typename I>
    COMPtr<I> Query() const noexcept {
        COMPtr<I> result;
        inner_->QueryInterface(__uuidof(I), result.ResetAsVoid());
        return result;
    }

    template<typename I>
    COMPtr<I> Query(const IID& iid) const noexcept {
        COMPtr<I> result;
        inner_->QueryInterface(iid, result.ResetAsVoid());
        return result;
    }

    T* Inner() const noexcept {
        return inner_;
    }

private:
    void CopyFrom(T* inner) {
        inner_ = inner;
        if (inner_) {
            inner_->AddRef();
        }
    }

private:
    T* inner_;
};


template<typename Class, typename... ArgumentTypes>
COMPtr<Class> MakeCOMPtr(ArgumentTypes&&... arguments) {
    return COMPtr<Class>{ new Class(std::forward<ArgumentTypes>(arguments)...) };
}


/**
Converts a raw COM pointer to COMPtr, doesn't call AddRef() on the pointer. That is, transfers the 
ownership of the pointer to COMPtr.
*/
template<typename T>
COMPtr<T> ToCOMPtr(T* ptr) noexcept {
    return COMPtr<T>{ ptr };
}


/**
Converts a raw COM pointer to COMPtr, calls AddRef() on the pointer. That is, the ownership of the
pointer isn't transferred.
*/
template<typename T>
COMPtr<T> ToCOMPtrNotOwn(T* ptr) noexcept {
    ptr->AddRef();
    return COMPtr<T>{ ptr };
}

}