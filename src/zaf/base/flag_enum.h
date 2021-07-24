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


namespace zaf {

template<typename T>
struct IsFlagEnum {
private:
    template<typename = std::enable_if_t<
        std::is_enum_v<T> && 
        std::is_same_v<
            decltype(std::declval<T>() & std::declval<T>()),
            decltype(std::declval<T>() | std::declval<T>())
        >
    >>
    static constexpr bool Test(T*) {
        return true;
    }

    static constexpr bool Test(...) {
        return false;
    }

public:
    static constexpr bool Value = Test(nullptr);
};

}