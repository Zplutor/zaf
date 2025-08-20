#pragma once

/**
@file
    Defines the `zaf::rx::DisposeBag` class.
*/

#include <mutex>
#include <unordered_set>
#include <zaf/base/non_copyable.h>

namespace zaf::rx {

class Disposable;

/**
Represents a container that disposes added `Disposable` objects when cleared or destroyed.
*/
class DisposeBag : NonCopyableNonMovable {
public:
    /**
    Constructs an empty instance.
    */
    DisposeBag() noexcept;

    /**
    Destructs the instance, disposing all added `Disposable` objects.

    @see Clear()
    */
    ~DisposeBag();

    /**
    Adds a `Disposable` object to the bag.

    @param disposable
        The `Disposable` object to add. The object won't be added if it is disposed or is null.

    @throw std::bad_alloc

    @details
        This method is thread-safe.
    */
    void Add(const std::shared_ptr<Disposable>& disposable);

    /**
    @copydoc Add(const std::shared_ptr<Disposable>&)

    @details
        This is a convenient operator for adding a `Disposable` object to the bag.

    @see Add(const std::shared_ptr<rx::Disposable>&)
    */
    void operator+=(const std::shared_ptr<Disposable>& disposable);

    /**
    Clears and disposes all `Disposable` objects in the bag.

    @details
        @note
            The order of disposal is not guaranteed.

        This method is thread-safe.
    */
    void Clear() noexcept;

    /**
    Gets the number of `Disposable` objects in the bag.
    */
    std::size_t Count() const noexcept;

private:
    mutable std::mutex lock_;
    std::unordered_set<std::shared_ptr<Disposable>> items_;
};

}