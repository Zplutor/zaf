#pragma once

#include <type_traits>

#define ZAF_ENABLE_FLAG_ENUM(Enum)                                                  \
inline Enum operator~(Enum value) {                                                 \
    return static_cast<Enum>(~static_cast<std::underlying_type_t<Enum>>(value));    \
}                                                                                   \
inline Enum operator|(Enum value1, Enum value2) {                                   \
    return static_cast<Enum>(                                                       \
        static_cast<std::underlying_type_t<Enum>>(value1) |                         \
        static_cast<std::underlying_type_t<Enum>>(value2));                         \
}                                                                                   \
inline Enum operator&(Enum value1, Enum value2) {                                   \
    return static_cast<Enum>(                                                       \
        static_cast<std::underlying_type_t<Enum>>(value1) &                         \
        static_cast<std::underlying_type_t<Enum>>(value2));                         \
}                                                                                   \
inline Enum operator^(Enum value1, Enum value2) {                                   \
    return static_cast<Enum>(                                                       \
        static_cast<std::underlying_type_t<Enum>>(value1) ^                         \
        static_cast<std::underlying_type_t<Enum>>(value2));                         \
}                                                                                   \
inline Enum& operator|=(Enum& value1, Enum value2) {                                \
    return reinterpret_cast<Enum&>(                                                 \
        reinterpret_cast<std::underlying_type_t<Enum>&>(value1) |=                  \
        static_cast<std::underlying_type_t<Enum>>(value2));                         \
}                                                                                   \
inline Enum& operator&=(Enum& value1, Enum value2) {                                \
    return reinterpret_cast<Enum&>(                                                 \
        reinterpret_cast<std::underlying_type_t<Enum>&>(value1) &=                  \
        static_cast<std::underlying_type_t<Enum>>(value2));                         \
}                                                                                   \
inline Enum& operator^=(Enum& value1, Enum value2) {                                \
    return reinterpret_cast<Enum&>(                                                 \
        reinterpret_cast<std::underlying_type_t<Enum>&>(value1) ^=                  \
        static_cast<std::underlying_type_t<Enum>>(value2));                         \
}