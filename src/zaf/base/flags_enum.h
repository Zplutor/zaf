#pragma once

/**
@file
    Defines macros, templates and functions related to flags enum.
*/

#include <type_traits>

/**
Enables the specified enum to be used as a flags enum.

@details
    This macro defines the following bitwise operators for the enum: 
    - ~ (bitwise NOT)
    - | (bitwise OR)
    - & (bitwise AND)
    - ^ (bitwise XOR)
    - |= (bitwise OR assignment)
    - &= (bitwise AND assignment)
    - ^= (bitwise XOR assignment)

    Additionally, it defines the specialization of zaf__CustomIsFlagsEnum for the enum to mark it 
    as a flags enum.
*/
#define ZAF_ENABLE_FLAGS_ENUM(Enum)                                                               \
inline Enum operator~(Enum value) {                                                               \
    return static_cast<Enum>(~static_cast<std::underlying_type_t<Enum>>(value));                  \
}                                                                                                 \
inline Enum operator|(Enum value1, Enum value2) {                                                 \
    return static_cast<Enum>(                                                                     \
        static_cast<std::underlying_type_t<Enum>>(value1) |                                       \
        static_cast<std::underlying_type_t<Enum>>(value2));                                       \
}                                                                                                 \
inline Enum operator&(Enum value1, Enum value2) {                                                 \
    return static_cast<Enum>(                                                                     \
        static_cast<std::underlying_type_t<Enum>>(value1) &                                       \
        static_cast<std::underlying_type_t<Enum>>(value2));                                       \
}                                                                                                 \
inline Enum operator^(Enum value1, Enum value2) {                                                 \
    return static_cast<Enum>(                                                                     \
        static_cast<std::underlying_type_t<Enum>>(value1) ^                                       \
        static_cast<std::underlying_type_t<Enum>>(value2));                                       \
}                                                                                                 \
inline Enum& operator|=(Enum& value1, Enum value2) {                                              \
    return reinterpret_cast<Enum&>(                                                               \
        reinterpret_cast<std::underlying_type_t<Enum>&>(value1) |=                                \
        static_cast<std::underlying_type_t<Enum>>(value2));                                       \
}                                                                                                 \
inline Enum& operator&=(Enum& value1, Enum value2) {                                              \
    return reinterpret_cast<Enum&>(                                                               \
        reinterpret_cast<std::underlying_type_t<Enum>&>(value1) &=                                \
        static_cast<std::underlying_type_t<Enum>>(value2));                                       \
}                                                                                                 \
inline Enum& operator^=(Enum& value1, Enum value2) {                                              \
    return reinterpret_cast<Enum&>(                                                               \
        reinterpret_cast<std::underlying_type_t<Enum>&>(value1) ^=                                \
        static_cast<std::underlying_type_t<Enum>>(value2));                                       \
}                                                                                                 \
template<>                                                                                        \
struct zaf__CustomIsFlagsEnum<Enum> : std::true_type {};


/**
A template to define a custom enum as a flags enum.

@tparam DecayedT
    The enum type, which should be a decayed type.

@details
    This template is used by zaf::IsFlagsEnum to check if an enum is a flags enum. A specialization 
    should be defined for the enum to mark it as a flags enum. Typically, users should use 
    ZAF_ENABLE_FLAGS_ENUM macro to define a flags enum rather than defining the specialization
    themselves.
*/
template<typename DecayedT>
struct zaf__CustomIsFlagsEnum : std::false_type { };


namespace zaf {

/**
A template to check if the specified type is a flags enum.

@tparam T
    The type to check.

@details
    `T` is a flags enum if:
    - `std::is_enum_v<std::decay_t<T>>` is true, and
    - `zaf__CustomIsFlagsEnum<std::decay_t<T>>::value` is true.
*/
template<typename T>
struct IsFlagsEnum {
private:
    using DecayedT = std::decay_t<T>;

public:
    /**
    Indicates whether `T` is a flags enum.
    */
    static constexpr bool value = 
        std::is_enum_v<DecayedT> && 
        zaf__CustomIsFlagsEnum<DecayedT>::value;
};

/**
A helper variable template for zaf::IsFlagsEnum.

@relates zaf::IsFlagsEnum
*/
template<typename T>
constexpr bool IsFlagsEnumV = IsFlagsEnum<T>::value;


/**
Checks whether the specified flags enum value has the specified flag.

@tparam T 
    The flags enum type.

@param value
    The enum value to check.

@param flag
    The flag to check.
    
@return
    Returns true if `value` has `flag`; otherwise false.
*/
template<typename T>
bool HasFlag(T value, T flag) {
    return (value & flag) == flag;
}

}