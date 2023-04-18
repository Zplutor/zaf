#pragma once

#include <dwrite.h>
#include <zaf/base/com_object.h>

namespace zaf {

class TextInlineObject : public COMObject<IDWriteInlineObject> {
public:
    TextInlineObject() { }
    TextInlineObject(IDWriteInlineObject* handle) : COMObject(handle) { }
};

}