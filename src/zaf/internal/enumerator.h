#pragma once

#include <functional>
#include <iterator>

namespace zaf {
namespace internal {

template<typename Container, typename Element>
class ComContainerEnumerator {
public:
    typedef std::function<std::size_t(const Container&)> CountGetter;
    typedef std::function<Element(const Container&, std::size_t)> ElementGetter;

    class Iterator : public std::iterator<std::input_iterator_tag, Element> {
    public:
        Iterator(const ComContainerEnumerator& enumerator, std::size_t index) :
            enumerator_(enumerator),
            index_(index) {

        }

        bool operator!=(const Iterator& other) {
            return (&enumerator_ != &other.enumerator_) || (index_ != other.index_);
        }

        Element operator*() {
            return enumerator_.element_getter_(enumerator_.container_, index_);
        }

        Element operator->() {
            return *(*this);
        }

        Iterator& operator++() {
            ++index_;
            return *this;
        }

        Iterator& operator++(difference_type) {
            ++index_;
            return *this;
        }

    private:
        const ComContainerEnumerator& enumerator_;
        std::size_t index_;
    };

public:
    ComContainerEnumerator(const Container& container, const CountGetter& count_getter, const ElementGetter& element_getter) :
        container_(container),
        count_getter_(count_getter),
        element_getter_(element_getter) {
    
    }

    Iterator begin() {
        return Iterator(*this, 0);
    }

    Iterator end() {
        return Iterator(*this, count_getter_(container_));
    }

private:
    const Container& container_;
    CountGetter count_getter_;
    ElementGetter element_getter_;
};

}
}