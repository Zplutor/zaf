#pragma once

namespace zaf {

enum class WindowHandleState {
    NotCreated,
    Creating,
    Created,
    Destroying,
    Destroyed,
};

}