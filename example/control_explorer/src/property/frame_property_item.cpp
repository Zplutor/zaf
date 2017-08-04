#include "property/frame_property_item.h"
#include "property/number_property_item.h"

std::shared_ptr<PropertyItem> CreateFramePropertyItem(
    const std::wstring& title,
    const std::function<zaf::Frame()>& get_frame,
    const std::function<void(const zaf::Frame&)>& frame_change) {

    auto title_item = CreatePropertyItem(title, nullptr);

    auto left_item = CreateFloatPropertyItem(
        L"    Left",
        [get_frame]() { return get_frame().left; },
        [get_frame, frame_change](float value) { 
            auto frame = get_frame();
            frame.left = value;
            frame_change(frame);
        }
    );

    auto top_item = CreateFloatPropertyItem(
        L"    Top",
        [get_frame]() { return get_frame().top; },
        [get_frame, frame_change](float value) {
            auto frame = get_frame();
            frame.top = value;
            frame_change(frame);
        }
    );

    auto right_item = CreateFloatPropertyItem(
        L"    Right",
        [get_frame]() { return get_frame().right; },
        [get_frame, frame_change](float value) {
            auto frame = get_frame();
            frame.right = value;
            frame_change(frame);
        }
    );

    auto bottom_item = CreateFloatPropertyItem(
        L"    Bottom",
        [get_frame]() { return get_frame().bottom; },
        [get_frame, frame_change](float value) {
            auto frame = get_frame();
            frame.bottom = value;
            frame_change(frame);
        }
    );

    return CreatePropertyItem({
        title_item,
        left_item,
        top_item,
        right_item,
        bottom_item,
    });
}