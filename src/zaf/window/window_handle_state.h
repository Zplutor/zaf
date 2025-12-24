#pragma once

/**
@file
    Defines the `zaf::WindowHandleState` enum.
*/

namespace zaf {

/**
Represents the state of a window handle.
*/
enum class WindowHandleState {

    /**
    The window handle is not created.

    @details
        When in this state, the window is prepared for creation. Calling modifications methods such 
        as setting styles or size are allowed, but calling methods that require a valid window 
        handle will throw `zaf::InvalidHandleStateError`.
    */
    NotCreated,

    /**
    The window handle is being created.

    @details
        When in this state, the window is already usable, but calling a few modification methods 
        will throw `zaf::InvalidHandleStateError`.
    */
    Creating,

    /**
    The window handle has been created.
    */
    Created,

    /**
    The window handle is being destroyed.

    @details
        When in this state, the window is still usable, calling retrieval methods of the window 
        will get valid results, but calling some modification methods will throw 
        `zaf::InvalidHandleStateError`.
    */
    Destroying,

    /**
    The window handle has been destroyed.

    @details
        When in this state, the window is unusable, calling most retrieval methods of the window 
        will get invalid results, and calling most modification methods will throw 
        `zaf::InvalidHandleStateError`.
    */
    Destroyed,
};

}