#pragma once

#include <dwrite.h>
#include <zaf/base/com_ptr.h>

namespace zaf {

class TextInlineObject : public COMPtr<IDWriteInlineObject> {
public:
    TextInlineObject() { }
    TextInlineObject(IDWriteInlineObject* handle) : COMPtr(handle) { }
};

}