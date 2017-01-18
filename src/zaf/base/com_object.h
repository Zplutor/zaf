#pragma once

namespace zaf {

template<typename ObjectType>
class ComObject {
public:
    ComObject() : handle_(nullptr) { 

    }

    explicit ComObject(ObjectType* handle) : handle_(handle) {
    
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
        if (handle_ != nullptr) {
            handle_->Release();
        }
    }

    ComObject& operator=(const ComObject& other) {
        handle_ = other.handle_;
        if (handle_ != nullptr) {
            handle_->AddRef();
        }
        return *this;
    }

    ComObject& operator=(ComObject&& other) {
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
        }
        handle_ = nullptr;
    }

    bool IsNull() const {
        return handle_ == nullptr;
    }

    ObjectType* GetHandle() const {
        return handle_;
    }

private:
    ObjectType* handle_;
};

}