#pragma once

namespace zaf {

class None {
public:
    friend bool operator==(None, None) {
        return false;
    }
};

}