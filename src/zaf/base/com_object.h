#pragma once

#include <unknwn.h>
#include <type_traits>

namespace zaf {

template<typename T>
class COMObject {
public:
    static_assert(std::is_base_of_v<IUnknown, T>);

    COMObject() : inner_(nullptr) { 

    }

    explicit COMObject(T* inner) : inner_(inner) {

    }

    virtual ~COMObject() {
        Reset();
    }

    COMObject(const COMObject& other) : inner_(other.inner_) {
        if (inner_) {
            inner_->AddRef();
        }
    }

    COMObject(COMObject&& other) : inner_(other.inner_) {
        other.inner_ = nullptr;
    }

    COMObject& operator=(const COMObject& other) {
        Reset();
        inner_ = other.inner_;
        if (inner_) {
            inner_->AddRef();
        }
        return *this;
    }

    COMObject& operator=(COMObject&& other) {
        Reset();
        inner_ = other.inner_;
        other.inner_ = nullptr;
        return *this;
    }

    bool operator==(std::nullptr_t null) const {
        return !IsValid();
    }

    bool operator!=(std::nullptr_t null) const {
        return IsValid();
    }

    bool operator==(const COMObject& other) const {
        return inner_ == other.inner_;
    }

    bool operator!=(const COMObject& other) const {
        return inner_ != other.inner_;
    }

    explicit operator bool() const {
        return IsValid();
    }

    T* operator->() const {
        return inner_;
    }

    void Reset() {
        Reset(nullptr);
    }

    void Reset(T* new_inner) {
        if (inner_) {
            inner_->Release();
        }
        inner_ = new_inner;
    }

    bool IsValid() const {
        return !!inner_;
    }

    T* Inner() const {
        return inner_;
    }

private:
    T* inner_;
};

}