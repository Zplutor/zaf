#pragma once

#include <atomic>
#include <cassert>
#include <cstdint>
#include <functional>
#include <thread>
#include <memory>
#include <mutex>

namespace zaf {

enum class LazyLock {
    Spin = 0,
    Mutex = 1,
};

namespace internal {

const std::uintptr_t NotCreatedState = 0;
const std::uintptr_t CreatingState = 1;
const std::uintptr_t CreatedState = 2;


class BaseLazyState {
public:
    BaseLazyState() : value(NotCreatedState) { }

    //Disable copying and also moving.
    BaseLazyState(const BaseLazyState&) = delete;
    BaseLazyState& operator=(const BaseLazyState&) = delete;

public:
    std::atomic<std::uintptr_t> value;
};


typedef std::function<void(std::uintptr_t state_value)> LazyInstanceCreator;


template<LazyLock LockType>
class LazyState;


template<>
class LazyState<LazyLock::Spin> : public BaseLazyState {
public:
    void CreateInstance(LazyInstanceCreator instance_creator) {

        //Get current state.
        auto current_state = value.load();

        //Another thread has been created instance, nothing to do.
        if (current_state == CreatedState) {
            return;
        }
        //Another thread is creating instance, wait until it finishes.
        else if (current_state == CreatingState) {
            WaitCreateFinished();
        }
        //Maybe current thread can create instance, need further check.
        else {

            //Check again to ensure that state is not changed, and set it to creating.
            value.compare_exchange_strong(current_state, CreatingState);

            //Repeat the check.
            if (current_state == CreatedState) {
                return;
            }
            //Repeat the check.
            else if (current_state == CreatingState) {
                WaitCreateFinished();
            }
            //Finally, create instance in current thread.
            else {

                instance_creator(current_state);
                value.store(CreatedState);
            }
        }
    }

    void WaitCreateFinished() const {

        do {
            std::this_thread::yield();
        } while (value.load() == CreatingState);
    }
};


template<>
class LazyState<LazyLock::Mutex> : public BaseLazyState {
public:
    void CreateInstance(LazyInstanceCreator instance_creator) {

        //Get current state.
        auto current_state = value.load();

        //Check whether instance has been created. Need further check if not.
        if (current_state != CreatedState) {

            //Acquire lock. Block other threads, or wait for the other thread that acquiring lock.
            std::lock_guard<std::mutex> lock_guard(mutex_);

            /*
             Double check. Also set to creating state if current_state is not changed after acquiring lock.
             This happens only if instance is not being created.
             */
            bool is_succeeded = value.compare_exchange_strong(current_state, CreatingState);
            if (is_succeeded) {

                //Create instance.
                instance_creator(current_state);
                value.store(CreatedState);
            }
        }
    }

    void WaitCreateFinished() const {

        do {
            std::lock_guard<std::mutex> lock_guard(mutex_);
        } while (value.load() == CreatingState);
    }

private:
    mutable std::mutex mutex_;
};

}


template<typename Type, LazyLock LockType = LazyLock::Spin>
class Lazy {
public:
    typedef std::function<void(Type&)> Initializer;

public:
    Lazy() { }

    ~Lazy() {

        auto current_state = state_.value.load();
        switch (current_state) {
        case internal::NotCreatedState:
            //Instance has not been created and no initializer is set, nothing to do.
            break;
        case internal::CreatingState:
            //It is abnormal if instance is being created while the lazy is being destroyed.
            assert(false);
            break;
        case internal::CreatedState:
            //Instance has been created, destroy it.
            GetRawPointer()->~Type();
            break;
        default:
            //Instance has not been created and a initializer is set,
            //destroy the initializer.
            delete reinterpret_cast<Initializer*>(current_state);
            break;
        }
    }

    /**
     Set an initializer that initialize the underlying instance.
     */
    void SetInitializer(Initializer initializer) {

        auto current_state = state_.value.load();

        //Nothing to do if instance has been created or is being creating.
        if ((current_state == internal::CreatedState) || (current_state == internal::CreatingState)) {
            return;
        }

        //Copy the initializer as a new state.
        Initializer* copied_initializer = nullptr;
        if (initializer != nullptr) {
            copied_initializer = new Initializer(std::move(initializer));
        }
        auto new_state = reinterpret_cast<std::uintptr_t>(copied_initializer);

        //Store new state. Make sure the state is not changed yet.
        bool is_succeeded = state_.value.compare_exchange_strong(current_state, new_state);

        //Delete the copied initializer if failed.
        if (!is_succeeded) {
            if (copied_initializer != nullptr) {
                delete copied_initializer;
            }
        }
    }

    /**
     Returns a value indicates that whether the underlying instance has been created.
     If it is being creating, wait until it is finished.
     */
    bool IsInitialized() const {
        return IsInitialized(false);
    }

    /**
     Returns a value indicates that whether the underlying instance has been created.
     Uses a parameter to control whether to wait if the instance is being created.
     */
    bool IsInitialized(bool wait) const {

        auto state = state_.value.load();
        if (state == internal::CreatedState) {
            return true;
        }

        if (state == internal::CreatingState) {

            if (!wait) {
                return false;
            }

            state_.WaitCreateFinished();
            return true;
        }

        return false;
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

        state_.CreateInstance([this](std::uintptr_t state_value) {

            new (GetRawPointer()) Type();

            if (state_value != internal::NotCreatedState) {

                auto initializer = reinterpret_cast<Initializer*>(state_value);
                (*initializer)(*GetRawPointer());
                delete initializer;
            }
        });
    }

    Type* GetRawPointer() {
        return reinterpret_cast<Type*>(&instance_data_);
    }

private:
    internal::LazyState<LockType> state_;
    typename std::aligned_storage<sizeof(Type), std::alignment_of<Type>::value>::type instance_data_;
};

}