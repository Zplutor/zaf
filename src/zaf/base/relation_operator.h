#pragma once

#define ZAF_ENABLE_RELATION_OPERATOR(Type)                \
inline bool operator!=(const Type& o1, const Type& o2) {  \
    return !(o1 == o2);                                   \
}                                                         \
inline bool operator>(const Type& o1, const Type& o2) {   \
    return !(o1 < o2) && !(o1 == o2);                     \
}                                                         \
inline bool operator<=(const Type&o1, const Type& o2) {   \
    return !(o1 > o2);                                    \
}                                                         \
inline bool operator>=(const Type&o1, const Type& o2) {   \
    return !(o1 < o2);                                    \
}