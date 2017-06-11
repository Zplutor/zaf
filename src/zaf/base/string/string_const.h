#pragma once

#define ZAF_DECLARE_STRING_CONST(str) extern const wchar_t* const str
#define ZAF_DEFINE_STRING_CONST(str) const wchar_t* const str = L#str