#pragma once

namespace zaf {

template<typename T>
class COMObject {
public:
    COMObject() : handle_(nullptr) { 

    }

    explicit COMObject(T* handle) : handle_(handle) {
    
    }

    COMObject(const COMObject& other) : handle_(other.handle_) {
        if (handle_ != nullptr) {
            handle_->AddRef();
        }
    }

    COMObject(COMObject&& other) : handle_(other.handle_) {
        other.handle_ = nullptr;
    }

    virtual ~COMObject() {
        Reset();
    }

    COMObject& operator=(const COMObject& other) {
        Reset();
        handle_ = other.handle_;
        if (handle_ != nullptr) {
            handle_->AddRef();
        }
        return *this;
    }

    COMObject& operator=(COMObject&& other) {
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

    bool operator==(const COMObject& other) const {
        return handle_ == other.handle_;
    }

    bool operator!=(const COMObject& other) const {
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