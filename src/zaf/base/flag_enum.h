#pragma once

#define ZAF_ENABLE_FLAG_ENUM(Enum)                                                              \
inline Enum operator~(Enum value) {                                                             \
	return static_cast<Enum>(~static_cast<int>(value));                                         \
}                                                                                               \
inline Enum operator|(Enum value1, Enum value2) {                                               \
	return static_cast<Enum>(static_cast<int>(value1) | static_cast<int>(value2));              \
}                                                                                               \
inline Enum operator&(Enum value1, Enum value2) {                                               \
	return static_cast<Enum>(static_cast<int>(value1) & static_cast<int>(value2));              \
}                                                                                               \
inline Enum operator^(Enum value1, Enum value2) {                                               \
	return static_cast<Enum>(static_cast<int>(value1) ^ static_cast<int>(value2));              \
}                                                                                               \
inline Enum& operator|=(Enum& value1, Enum value2) {                                            \
	return reinterpret_cast<Enum&>(reinterpret_cast<int&>(value1) |= static_cast<int>(value2)); \
}                                                                                               \
inline Enum& operator&=(Enum& value1, Enum value2) {                                            \
	return reinterpret_cast<Enum&>(reinterpret_cast<int&>(value1) &= static_cast<int>(value2)); \
}                                                                                               \
inline Enum& operator^=(Enum& value1, Enum value2) {                                            \
	return reinterpret_cast<Enum&>(reinterpret_cast<int&>(value1) ^= static_cast<int>(value2)); \
}