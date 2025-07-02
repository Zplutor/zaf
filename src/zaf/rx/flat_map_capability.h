#pragma once

namespace zaf::rx {

template<
    template<typename> typename OBSERVABLE,
    typename T
>
class FlatMapCapability {
public:
    template<typename K>
    OBSERVABLE<K> FlatMap(std::function<OBSERVABLE<K>(const T&)> mapper) {

        const auto& core = static_cast<const OBSERVABLE<T>*>(this)->Core();
        auto new_core = core->FlatMap([mapper = std::move(mapper)](const std::any& value) {
            return mapper(std::any_cast<T>(value)).Core();
        });
        return OBSERVABLE<K>{ std::move(new_core) };
    }
};

}