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

    COMObject(const COMObject& other) {
        CopyFrom(other.Inner());
    }

    template<typename K, std::enable_if_t<std::is_base_of_v<T, K>, int> = 0>
    COMObject(const COMObject<K>& other) {
        CopyFrom(other.Inner());
    }

    COMObject(COMObject&& other) : inner_(other.Detach()) {

    }

    template<typename K, std::enable_if_t<std::is_base_of_v<T, K>, int> = 0>
    COMObject(COMObject<K>&& other) : inner_(other.Detach()) {

    }

    COMObject& operator=(const COMObject& other) {
        Reset();
        CopyFrom(other.Inner());
        return *this;
    }

    template<typename K, std::enable_if_t<std::is_base_of_v<T, K>, int> = 0>
    COMObject& operator=(const COMObject<K>& other) {
        Reset();
        CopyFrom(other.Inner());
        return *this;
    }

    COMObject& operator=(COMObject&& other) {
        Reset();
        inner_ = other.Detach();
        return *this;
    }

    template<typename K, std::enable_if_t<std::is_base_of_v<T, K>, int> = 0>
    COMObject& operator=(COMObject<K>&& other) {
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

    void** Reset() {
        Reset(nullptr);
        return reinterpret_cast<void**>(&inner_);
    }

    void Reset(T* new_inner) {
        if (inner_) {
            inner_->Release();
        }
        inner_ = new_inner;
    }

    T* Detach() {
        auto result = inner_;
        inner_ = nullptr;
        return result;
    }

    bool IsValid() const {
        return !!inner_;
    }

    template<typename K>
    COMObject<K> As() const {
        COMObject<K> result;
        inner_->QueryInterface(__uuidof(K), result.Reset());
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

}