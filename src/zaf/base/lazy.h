#pragma once

#include <atomic>
#include <functional>
#include <mutex>

namespace zaf {

template<typename Type>
class Lazy {
public:
    typedef std::function<void(Type& instance)> Initializer;

public:
    Lazy() : is_initialized_(false) { }

    ~Lazy() {

        std::lock_guard<std::mutex> lock_guard(lock_);

        if (is_initialized_.load()) {
            GetRawPointer()->~Type();
        }
    }

    /**
     Set an initializer that initialize the underlying instance.

     If the underlying instance has been created, initializer would not set.
     */
    void SetInitializer(Initializer initializer) {

        if (is_initialized_.load()) {
            return;
        }

        std::lock_guard<std::mutex> lock_guard(lock_);

        if (is_initialized_.load()) {
            return;
        }

        initializer_ = std::move(initializer);
    }

    /**
     Returns a value indicates that whether the underlying instance has been created.
     If it is being created, wait until finished.
     */
    bool IsInitialized() const {

        if (is_initialized_.load()) {
            return true;
        }

        std::lock_guard<std::mutex> lock_guard(lock_);
        return is_initialized_.load();
    }

    Type& GetInstance() {
        Instantiate();
        return *GetRawPointer();
    }

    Type* operator->() {
        return &GetInstance();
    }

    Type& operator*() {
        return GetInstance();
    }

    //Forbidden coping (and also moving).
    Lazy(const Lazy&) = delete;
    Lazy& operator=(const Lazy&) = delete;

private:
    void Instantiate() {

        if (is_initialized_.load()) {
            return;
        }

        std::lock_guard<std::mutex> lock_guard(lock_);

        if (is_initialized_.load()) {
            return;
        }
    
        new (GetRawPointer()) Type();

        if (initializer_ != nullptr) {
            initializer_(*GetRawPointer());
            initializer_ = nullptr;
        }

        is_initialized_.store(true);
    }

    Type* GetRawPointer() {
        return reinterpret_cast<Type*>(&instance_data_);
    }

private:
    typename std::aligned_storage<sizeof(Type), std::alignment_of<Type>::value>::type instance_data_;
    std::atomic<bool> is_initialized_;
    Initializer initializer_;
    mutable std::mutex lock_;
};

}