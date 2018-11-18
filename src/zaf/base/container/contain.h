#pragma once

#include <type_traits>

namespace zaf {
namespace internal {

template<typename C>
struct IsAssociativeContainer {
private:
	template<typename T>
	static constexpr bool Test(typename T::key_type*) { 
		return true; 
	}

	template<typename T>
	static constexpr bool Test(...) {
		return false;
	}

public:
	static constexpr bool Value = Test<C>(nullptr);
};

template<typename C, typename E>
struct FastImplement {
	static bool Contain(const C& container, const E& element) {
		return container.find(element) != container.end();
	}
};

template<typename C, typename E>
struct NormalImplement {
	static bool Contain(const C& container, const E& element) {
		return std::find(container.begin(), container.end(), element) != container.end();
	}
};

}


template<typename C, typename E>
bool Contain(const C& container, const E& element) {
	return std::conditional_t<
		internal::IsAssociativeContainer<C>::Value,
		internal::FastImplement<C, E>,
		internal::NormalImplement<C, E>
	>::Contain(container, element);
}

}