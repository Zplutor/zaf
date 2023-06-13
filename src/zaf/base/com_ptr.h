#pragma once

#include <unknwn.h>
#include <type_traits>

namespace zaf {

template<typename T>
class COMPtr {
public:
    static_assert(std::is_base_of_v<IUnknown, T>);

public:
    COMPtr() : inner_(nullptr) { 

    }

    COMPtr(std::nullptr_t) : inner_(nullptr) {

    }

    explicit COMPtr(T* inner) : inner_(inner) {

    }

    virtual ~COMPtr() {
        Reset();
    }

    COMPtr(const COMPtr& other) {
        CopyFrom(other.Inner());
    }

    template<typename K, std::enable_if_t<std::is_base_of_v<T, K>, int> = 0>
    COMPtr(const COMPtr<K>& other) {
        CopyFrom(other.Inner());
    }

    COMPtr(COMPtr&& other) : inner_(other.Detach()) {

    }

    template<typename K, std::enable_if_t<std::is_base_of_v<T, K>, int> = 0>
    COMPtr(COMPtr<K>&& other) : inner_(other.Detach()) {

    }

    COMPtr& operator=(const COMPtr& other) {
        if (this != &other) {
            Reset();
            CopyFrom(other.Inner());
        }
        return *this;
    }

    template<typename K, std::enable_if_t<std::is_base_of_v<T, K>, int> = 0>
    COMPtr& operator=(const COMPtr<K>& other) {
        Reset();
        CopyFrom(other.Inner());
        return *this;
    }

    COMPtr& operator=(COMPtr&& other) {
        if (this != &other) {
            Reset();
            inner_ = other.Detach();
        }
        return *this;
    }

    template<typename K, std::enable_if_t<std::is_base_of_v<T, K>, int> = 0>
    COMPtr& operator=(COMPtr<K>&& other) {
        Reset();
        inner_ = other.Detach();
        return *this;
    }

    bool operator==(std::nullptr_t null) const {
        return !IsValid();
    }

    bool operator!=(std::nullptr_t null) const {
        return IsValid();
    }

    bool operator==(const COMPtr& other) const {
        return inner_ == other.inner_;
    }

    bool operator!=(const COMPtr& other) const {
        return inner_ != other.inner_;
    }

    explicit operator bool() const {
        return IsValid();
    }

    T* operator->() const {
        return inner_;
    }

    T** Reset() {
        Reset(nullptr);
        return &inner_;
    }

    void** ResetAsVoid() {
        return reinterpret_cast<void**>(Reset());
    }

    void Reset(T* new_inner) {
        if (inner_ != new_inner) {
            if (inner_) {
                inner_->Release();
            }
            inner_ = new_inner;
        }
    }

    T* Detach() {
        auto result = inner_;
        inner_ = nullptr;
        return result;
    }

    bool IsValid() const {
        return !!inner_;
    }

    template<typename I>
    COMPtr<I> Query() const {
        COMPtr<I> result;
        inner_->QueryInterface(__uuidof(I), result.ResetAsVoid());
        return result;
    }

    template<typename C>
    COMPtr<C> As() const {
        COMPtr<C> result{ dynamic_cast<C*>(Inner()) };
        if (result) {
            result->AddRef();
        }
        return result;
    }

    T* Inner() const {
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