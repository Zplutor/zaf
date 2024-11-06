#pragma once

#include <memory>
#include <variant>
#include <zaf/base/as.h>

namespace zaf::internal {

/**
A pointer template that is used to resolve self-referencing issue.
*/
template<typename T>
class NoSelfRefPtr {
public:
    NoSelfRefPtr() = default;

    template<typename Owner>
    NoSelfRefPtr(std::shared_ptr<T> ptr, Owner* owner) {
        Assign<Owner>(std::move(ptr), owner);
    }

    template<typename Owner>
    void Assign(std::shared_ptr<T> ptr, Owner* owner) {

        auto owner_ptr = As<Owner>(ptr);
        if (owner_ptr.get() == owner) {
            variant_ = std::weak_ptr<T>(std::move(ptr));
        }
        else {
            variant_ = std::move(ptr);
        }
    }

    explicit operator bool() const noexcept {

        return std::visit([](const auto& ptr) {

            using PtrType = std::decay_t<decltype(ptr)>;
            if constexpr (std::is_same_v<PtrType, std::shared_ptr<T>>) {
                return !!ptr;
            }
            if constexpr (std::is_same_v<PtrType, std::weak_ptr<T>>) {
                return !ptr.expired();
            }
        },
        variant_);
    }

    std::shared_ptr<T> ToSharedPtr() const noexcept {

        return std::visit([](const auto& ptr) {

            using PtrType = std::decay_t<decltype(ptr)>;
            if constexpr (std::is_same_v<PtrType, std::shared_ptr<T>>) {
                return ptr;
            }
            if constexpr (std::is_same_v<PtrType, std::weak_ptr<T>>) {
                return ptr.lock();
            }
        },
        variant_);
    }

private:
    std::variant<std::shared_ptr<T>, std::weak_ptr<T>> variant_;
};

}
