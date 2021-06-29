#pragma once

#include <zaf/object/boxing/internal/get_box_type.h>
#include <zaf/object/object_type.h>

namespace zaf {

class ObjectParser;

template<typename T>
ObjectParser* GetObjectParser() {
	return internal::GetBoxType<T>::Type::Type->GetParser();
}

}