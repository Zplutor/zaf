#pragma once

#include <type_traits>
#include <cstdint>

namespace zaf {

/**
 Determinate whether the given type is a numerical type 
 (that is, an integral type or a floating-point type, but not a bool).
 */
template<typename Type>
class IsNumerical {
public:
    static const bool Value = std::is_arithmetic<Type>::value;
};

template<>
class IsNumerical<bool> {
public:
    static const bool Value = false;
};


class Number {
public:
    enum class Tag {
        SignedInteger,
        UnsignedInteger,
        FloatingPoint,
    };

public:
    Number() : tag_(Tag::SignedInteger) {
        value_.signed_integer = 0;
    }

    Number(const Number& other) {
        CopyFromOther(other);
    }

    template<typename Type, typename Tag = std::enable_if_t<IsNumerical<Type>::Value>>
    Number(Type value) {
        GetOperator<Type>::Type::Assign(this, value);
    }

    Number& operator=(const Number& other) {
        CopyFromOther(other);
        return *this;
    }

    template<typename Type, typename Tag = std::enable_if<IsNumerical<Type>::Value>>
    Number& operator=(Type value) {
        GetOperator<Type>::Type::Assign(this, value);
        return *this;
    }

    Tag GetTag() const {
        return tag_;
    }

    template<typename Type>
    Type GetValue() const {
        switch (tag_) {
        case Tag::SignedInteger:
            return static_cast<Type>(value_.signed_integer);
        case Tag::UnsignedInteger:
            return static_cast<Type>(value_.unsigned_integer);
        case Tag::FloatingPoint:
            return static_cast<Type>(value_.floating_point);
        default:
            return 0;
        }
    }

private:
    template<typename Type>
    class SignedIntegerOperator {
    public:
        static void Assign(Number* number, Type value) {
            number->tag_ = Tag::SignedInteger;
            number->value_.signed_integer = value;
        }
    };

    template<typename Type>
    class UnsignedIntegerOperator {
    public:
        static void Assign(Number* number, Type value) {
            number->tag_ = Tag::UnsignedInteger;
            number->value_.unsigned_integer = value;
        }
    };

    template<typename Type>
    class FloatingPointOperator {
    public:
        static void Assign(Number* number, Type value) {
            number->tag_ = Tag::FloatingPoint;
            number->value_.floating_point = value;
        }
    };

    template<typename Type>
    class GetOperator {
    public:
        typedef std::conditional_t<
            IsNumerical<Type>::Value,
            std::conditional_t<
                std::is_floating_point<Type>::value, 
                FloatingPointOperator<Type>,
                std::conditional_t<
                    std::is_signed<Type>::value,
                    SignedIntegerOperator<Type>,
                    UnsignedIntegerOperator<Type>
                >
            >,
            void
        > Type;
    };

private:
    void CopyFromOther(const Number& other) {
        tag_ = other.tag_;
        switch (tag_) {
        case Tag::SignedInteger:
            value_.signed_integer = other.value_.signed_integer;
            break;
        case Tag::UnsignedInteger:
            value_.unsigned_integer = other.value_.unsigned_integer;
            break;
        case Tag::FloatingPoint:
            value_.floating_point = other.value_.floating_point;
            break;
        }
    }

private:
    Tag tag_;
    union {
        std::int64_t signed_integer;
        std::uint64_t unsigned_integer;
        double floating_point;
    } value_;
};

}