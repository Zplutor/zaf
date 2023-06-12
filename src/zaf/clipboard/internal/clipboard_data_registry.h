#pragma once

#include <map>
#include <zaf/base/non_copyable.h>
#include <zaf/clipboard/clipboard_data.h>
#include <zaf/object/object_type.h>

namespace zaf::clipboard::internal {

class ClipboardDataRegistry : NonCopyableNonMovable {
public:
    static ClipboardDataRegistry& Instance();

    void RegisterType(FormatType format_type, ObjectType* data_type);
    ObjectType* GetRegisteredType(FormatType format_type) noexcept;

private:
    ClipboardDataRegistry();

private:
    std::map<FormatType, ObjectType*> registered_types_;
};

}