#pragma once

/**
@file
    Defines the class template zaf::COMObject.
*/

#include <zaf/base/com_ptr.h>

namespace zaf {

/**
A base class template for encapsulation classes over the specified COM interface, providing a 
consistent interface for these classes.

@details
    In zaf, there are many encapsulation classes for COM interfaces. They translate the methods of 
    the interfaces to more friendly C++ style methods. COMObject serves as the base class for all
    these classes. 
    
    The template argument I is the COM interface being encapsulated. 

    COMObject is nullable, similar to a pointer. Users should check if it is null before using it.

@note
    COMObject is not designed for polymorphism purpose; hence it doesn't have a virtual destructor.
*/
template<typename I>
class COMObject {
public:
    /**
    Constructs a null instance.
    */
    COMObject() noexcept = default;

    /**
    Constructs an instance from the specified COMPtr.

    @param ptr
        The COMPtr that the instance will own.
    */
    explicit COMObject(COMPtr<I> ptr) noexcept : ptr_(std::move(ptr)) { }

    /**
    Gets the underlying COMPtr of the COMObject.

    @return
        The COMPtr that the COMObject owns.
    */
    const COMPtr<I>& Ptr() const noexcept {
        return ptr_;
    }

    /**
    The bool conversion operator that indicates whether the COMObject is not null.

    @return
        Returns ture if the COMObject is not null; otherwise, false.
    */
    explicit operator bool() const noexcept {
        return ptr_.IsValid();
    }

    /**
    The equality operator with std::nullptr_t that checks whether the COMObject is null.

    @return
        Returns ture if the COMObject is null; otherwise, false.

    @relates COMObject
    */
    friend bool operator==(const COMObject& object, std::nullptr_t) noexcept {
        return !object;
    }

private:
    COMPtr<I> ptr_;
};

}