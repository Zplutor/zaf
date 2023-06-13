#pragma once

#include <zaf/base/com_ptr.h>
#include <zaf/base/direct2d.h>
#include <zaf/graphic/size.h>

namespace zaf {

class Layer : public COMPtr<ID2D1Layer> {
public:
    Layer() { }
    explicit Layer(ID2D1Layer* handle) : COMPtr(handle) { }

    const Size GetSize() const {
        return Size::FromD2D1SIZEF(Inner()->GetSize());
    }
};

}