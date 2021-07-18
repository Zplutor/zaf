#include <zaf/window/inspector/internal/inspect_data_source.h>
#include <zaf/creation.h>

namespace zaf::internal {

InspectDataSource::InspectDataSource(const std::shared_ptr<Window>& window) : window_(window) {

}


void InspectDataSource::ControlAddChild(const std::shared_ptr<Control>& parent) {

    auto old_child_count = parent->ChildCount();
    if (old_child_count > 0) {
        old_child_count--;
    }
    NotifyDataAdd(parent, old_child_count, 1);
}


void InspectDataSource::ControlRemoveChild(
    const std::shared_ptr<Control>& parent, 
    std::size_t removed_index) {

    NotifyDataRemove(parent, removed_index, 1);
}


bool InspectDataSource::DoesDataHasChildren(const std::shared_ptr<Object>& data) {

    auto window = dynamic_cast<Window*>(data.get());
    if (window) {
        return true;
    }

    auto control = dynamic_cast<Control*>(data.get());
    if (control) {
        return control->HasChildren();
    }

    return false;
}


std::size_t InspectDataSource::GetChildDataCount(const std::shared_ptr<Object>& parent_data) {

    if (!parent_data) {
        return 1;
    }

    auto window = dynamic_cast<Window*>(parent_data.get());
    if (window) {
        return 1;
    }

    auto control = dynamic_cast<Control*>(parent_data.get());
    if (control) {
        return control->ChildCount();
    }

    return 0;
}


std::shared_ptr<Object> InspectDataSource::GetChildDataAtIndex(
    const std::shared_ptr<Object>& parent_data,
    std::size_t index) {

    if (!parent_data) {
        return window_;
    }

    auto window = dynamic_cast<Window*>(parent_data.get());
    if (window) {
        return window->GetRootControl();
    }

    auto control = dynamic_cast<Control*>(parent_data.get());
    if (control) {
        return control->GetChildAtIndex(index);
    }

    return Object::Dumb();
}

}