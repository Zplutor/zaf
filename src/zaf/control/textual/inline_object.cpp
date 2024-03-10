#include <zaf/control/textual/inline_object.h>
#include <zaf/base/as.h>
#include <zaf/object/type_definition.h>

namespace zaf::textual {

ZAF_DEFINE_TYPE(InlineObject)
ZAF_DEFINE_TYPE_END;

TextInlineObjectMetrics InlineObject::GetMetrics() const {
    return {};
}


void InlineObject::OnAttached(const AttachedInfo& event_info) {
    attached_event_.Raise(event_info);
}


void InlineObject::OnDetached(const DetachedInfo& event_info) {
    detached_event_.Raise(event_info);
}


void InlineObject::Paint(Canvas& canvas) const {

}


void InlineObject::SetHost(std::shared_ptr<TextualControl> host) {

    if (host) {

        ZAF_EXPECT(host_.expired());
        host_ = std::move(host);
        OnAttached(AttachedInfo{ As<InlineObject>(shared_from_this()) });
    }
    else {

        host_.reset();
        OnDetached(DetachedInfo{ As<InlineObject>(shared_from_this()) });
    }
}

}