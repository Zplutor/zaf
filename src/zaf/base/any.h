#pragma once

#include <memory>
#include <type_traits>

namespace zaf {

class any {
public:
    any() { }

    any(const any& other) : holder_(other.holder_->clone()) {

    }

    any(any&& other) : holder_(std::move(other.holder_)) {

    }

    template<
        typename T, 
        typename Tag = std::enable_if_t<!std::is_same<any, std::remove_cv_t<std::decay_t<T>>>::value>>
    any(T&& value) : holder_(create_holder(std::forward<T>(value))) {

    }

    any& operator=(const any& other) {
        holder_ = other.holder_->clone();
        return *this;
    }

    any& operator=(any&& other) {
        holder_ = std::move(other.holder_);
        return *this;
    }

    template<
        typename T,
        typename Tag = std::enable_if_t<!std::is_same<any, std::remove_cv_t<std::decay_t<T>>>::value>>
    any& operator=(T&& value) {
        holder_ = create_holder(std::forward<T>(value));
        return *this;
    }

    void swap(any& other) {
        std::swap(holder_, other.holder_);
    }

    void reset() {
        holder_.reset();
    }

    bool has_value() const {
        return holder_ != nullptr;
    }

    const type_info& type() const {
        if (holder_ != nullptr) {
            return holder_->type();
        }
        return typeid(void);
    }

private:
    template<typename T>
    friend T* any_cast(any*);

    template<typename T>
    T* get_value_pointer() {
        auto holder = dynamic_cast<get_holder<T>::type*>(holder_.get());
        if (holder != nullptr) {
            return holder->get_value_pointer();
        }
        return nullptr;
    }

private:
    class holder {
    public:
        holder() { }
        virtual ~holder() { }

        virtual std::unique_ptr<holder> clone() const = 0;
        virtual const type_info& type() const = 0;

        holder(const holder&) = delete;
        holder& operator=(const holder&) = delete;
    };

    template<typename T>
    class typed_holder : public holder {
    public:
        typed_holder(const T& value) : value_(value) { }
        typed_holder(T&& value) : value_(std::forward<T>(value)) { }

        std::unique_ptr<holder> clone() const override {
            return create_holder(value_);
        }

        const type_info& type() const override {
            return typeid(T);
        }

        T* get_value_pointer() {
            return &value_;
        }

    private:
        T value_;
    };

    template<typename T>
    struct get_holder {
        typedef typed_holder<std::remove_cv_t<std::decay_t<T>>> type;
    };

private:
    template<typename T>
    static std::unique_ptr<holder> create_holder(T&& value) {
        return std::make_unique<get_holder<T>::type>(std::forward<T>(value));
    }

private:
    std::unique_ptr<holder> holder_;
};


class bad_any_cast : public std::bad_cast {

};


template<typename T>
T* any_cast(any* any) {

    if (any != nullptr) {
        return any->get_value_pointer<T>();
        
    }
    return nullptr;
}

template<typename T>
const T* any_cast(const any* any_pointer) {
    return any_cast<T>(const_cast<any*>(any_pointer));
}

template<typename T>
T any_cast(const any& any) {

    auto value = any_cast<T>(&any);
    if (value != nullptr) {
        return *value;
    }

    throw bad_any_cast();
}

template<typename T>
T any_cast(any&& any) {

    T* value = any_cast<T>(&any);
    if (value != nullptr) {
        return std::move(*value);
    }

    throw bad_any_cast();
}


}