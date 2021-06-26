#pragma once

namespace zaf {

template<typename T>
class ComObject {
public:
    ComObject() : handle_(nullptr) { 

    }

    explicit ComObject(T* handle) : handle_(handle) {
    
    }

    ComObject(const ComObject& other) : handle_(other.handle_) {
        if (handle_ != nullptr) {
            handle_->AddRef();
        }
    }

    ComObject(ComObject&& other) : handle_(other.handle_) {
        other.handle_ = nullptr;
    }

    virtual ~ComObject() {
        Reset();
    }

    ComObject& operator=(const ComObject& other) {
        Reset();
        handle_ = other.handle_;
        if (handle_ != nullptr) {
            handle_->AddRef();
        }
        return *this;
    }

    ComObject& operator=(ComObject&& other) {
        Reset();
        handle_ = other.handle_;
        other.handle_ = nullptr;
        return *this;
    }

    bool operator==(std::nullptr_t null) const {
        return IsNull();
    }

    bool operator!=(std::nullptr_t null) const {
        return ! IsNull();
    }

    bool operator==(const ComObject& other) const {
        return handle_ == other.handle_;
    }

    bool operator!=(const ComObject& other) const {
        return handle_ != other.handle_;
    }

    void Reset() {
        if (handle_ != nullptr) {
            handle_->Release();
            handle_ = nullptr;
        }
    }

    bool IsNull() const {
        return handle_ == nullptr;
    }

    T* GetHandle() const {
        return handle_;
    }

private:
    T* handle_;
};

}