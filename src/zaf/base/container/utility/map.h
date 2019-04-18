#pragma once

#include <vector>

namespace zaf {
namespace internal {

template<typename C>
struct KeyAccessor {
    using Type = typename C::key_type;
    static const Type& Access(typename C::const_iterator iterator) {
        return iterator->first;
    }
};

template<typename C>
struct ValueAccessor {
    using Type = typename C::mapped_type;
    static const Type& Access(typename C::const_iterator iterator) {
        return iterator->second;
    }
};


template<typename C, template<typename> typename IteratorAccessor>
class MapView {
public:
    using value_type = typename IteratorAccessor<C>::Type;

    class Iterator : public std::iterator<std::bidirectional_iterator_tag, value_type> {
    public:
        Iterator(typename C::const_iterator map_iterator) : map_iterator_(std::move(map_iterator)) { }

        Iterator& operator++() {
            ++map_iterator_;
            return *this;
        }

        Iterator operator++(int) {
            auto preivous_map_iterator = iterator_++;
            return Iterator(preivous_map_iterator);
        }

        Iterator& operator--() {
            --map_iterator_;
            return *this;
        }

        Iterator operator--(int) {
            auto previous_map_iterator = iterator--;
            return Iterator(previous_map_iterator);
        }

        const value_type& operator*() const {
            return IteratorAccessor<C>::Access(map_iterator_);
        }

        bool operator!=(const Iterator& other) const {
            return this->map_iterator_ != other.map_iterator_;
        }

    private:
        typename C::const_iterator map_iterator_;
    };

public:
    MapView(const C& map) : map_(map) { }

    MapView(const MapView&) = default;
    MapView& operator=(const MapView&) = default;

    MapView(MapView&&) = default;
    MapView& operator=(MapView&&) = default;

    Iterator begin() const {
        return Iterator(map_.begin());
    }

    Iterator end() const {
        return Iterator(map_.end());
    }

private:
    const C& map_;
};

}

template<typename C>
using KeysView = internal::MapView<C, internal::KeyAccessor>;

template<typename C>
using ValuesView = internal::MapView<C, internal::ValueAccessor>;


template<typename C>
KeysView<C> Keys(const C& map) {
    return KeysView<C>(map);
}


template<typename C>
ValuesView<C> Values(const C& map) {
    return ValuesView<C>(map);
}


template<typename C>
std::vector<typename C::key_type> GetKeys(const C& map) {
    std::vector<typename C::key_type> keys;
    for (const auto& key : Keys(map)) {
        keys.push_back(key);
    }
    return keys;
}


template<typename C>
std::vector<typename C::mapped_type> GetValues(const C& map) {
    std::vector<typename C::mapped_type> values;
    for (const auto& value : Values(map)) {
        values.push_back(value);
    }
    return values;
}

}