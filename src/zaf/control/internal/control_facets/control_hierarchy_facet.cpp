#include <zaf/control/internal/control_facets/control_hierarchy_facet.h>
#include <zaf/application.h>
#include <zaf/control/control.h>
#include <zaf/window/inspector/internal/inspector_port.h>
#include <zaf/window/window.h>

namespace zaf::internal {
namespace {

// Remove this function once a suitable place is found.
float GetDPIFromWindow(const std::shared_ptr<Window>& window) {
    if (window) {
        return window->DPI();
    }
    return Application::Instance().GetSystemDPI();
}

}

ControlHierarchyFacet::ControlHierarchyFacet(Control& control) noexcept : control_(control) {

}


std::shared_ptr<zaf::Window> ControlHierarchyFacet::Window() const noexcept {
    auto parent = this->Parent();
    if (parent) {
        return parent->Window();
    }
    return DirectWindow();
}


std::shared_ptr<zaf::Window> ControlHierarchyFacet::DirectWindow() const noexcept {
    return window_.lock();
}


void ControlHierarchyFacet::SetDirectWindow(const std::shared_ptr<zaf::Window>& window) noexcept {
    window_ = window;
}


std::shared_ptr<Control> ControlHierarchyFacet::Parent() const noexcept {
    return parent_.lock();
}


void ControlHierarchyFacet::SetParent(const std::shared_ptr<Control>& parent) {

    auto previous_parent = parent_.lock();
    if (previous_parent == parent) {
        return;
    }

    std::shared_ptr<zaf::Window> previous_window;
    if (previous_parent) {
        previous_window = previous_parent->Window();
    }

    std::shared_ptr<zaf::Window> new_window;
    if (parent) {
        new_window = parent->Window();
    }

    bool need_release_renderer_resources = [&previous_window, &new_window]() {
        if (!previous_window) {
            return false;
        }
        if (!new_window) {
            return true;
        }
        return previous_window != new_window;
    }();

    if (need_release_renderer_resources) {
        control_.ReleaseRendererResources();
    }

    parent_ = parent;

    if (parent) {
        control_.NeedUpdateStyle();
    }

    ParentChangedInfo parent_changed_info{ control_.shared_from_this(), previous_parent };
    control_.OnParentChanged(parent_changed_info);

    if (previous_window != new_window) {
        control_.RaiseWindowChangedEvent(previous_window);
    }

    if (GetDPIFromWindow(previous_window) != GetDPIFromWindow(new_window)) {
        control_.RaiseDPIChangedEvent();
    }
}


bool ControlHierarchyFacet::IsParentOf(const Control& control) const noexcept {
    return control.Parent().get() == &control_;
}


bool ControlHierarchyFacet::IsAncestorOf(const Control& control) const noexcept {

    auto ancestor = control.Parent();
    while (ancestor) {
        if (ancestor.get() == &control_) {
            return true;
        }
        ancestor = ancestor->Parent();
    }
    return false;
}


bool ControlHierarchyFacet::IsSameOrAncestorOf(const Control& control) const noexcept {
    return (&control_ == &control) || this->IsAncestorOf(control);
}


std::shared_ptr<Control> ControlHierarchyFacet::GetChildAtIndex(std::size_t index) const noexcept {
    if (index < children_.size()) {
        return children_[index];
    }
    return nullptr;
}


const std::vector<std::shared_ptr<Control>>& ControlHierarchyFacet::Children() const noexcept {
    return children_;
}


void ControlHierarchyFacet::SetChildren(const std::vector<std::shared_ptr<Control>>& children) {
    auto update_guard = control_.BeginUpdate();
    RemoveAllChildren();
    AddChildren(children);
}


void ControlHierarchyFacet::AddChild(const std::shared_ptr<Control>& child) {
    AddChildAtIndex(child, children_.size());
}


void ControlHierarchyFacet::AddChildAtIndex(
    const std::shared_ptr<Control>& child,
    std::size_t index) {

    ZAF_EXPECT(child);

    auto previous_parent = child->Parent();
    if (previous_parent) {

        // If the child is already added to this control, do nothing.
        if (previous_parent.get() == &control_) {
            return;
        }
    
        // Remove the child from its previous parent.
        previous_parent->hierarchy_facet_->InnerRemoveChild(child, false);
    }

    auto revised_index = std::min(index, children_.size());

    auto insert_iterator = std::next(children_.begin(), revised_index);
    children_.insert(insert_iterator, child);

    child->SetParent(control_.shared_from_this());

    control_.RequestLayout();
    control_.NeedRepaintRect(child->Rect());

    control_.AutoResizeToPreferredSize();

    auto inspector_port = control_.GetInspectorPort();
    if (inspector_port) {
        inspector_port->ControlAddChild(control_.shared_from_this(), index);
    }
}


void ControlHierarchyFacet::AddChildren(const std::vector<std::shared_ptr<Control>>& children) {

    auto update_guard = control_.BeginUpdate();

    for (const auto& each_child : children) {
        AddChild(each_child);
    }
}


void ControlHierarchyFacet::AddChildrenAtIndex(
    const std::vector<std::shared_ptr<Control>>& children,
    std::size_t index) {

    auto update_guard = control_.BeginUpdate();

    for (auto iterator = children.rbegin(); iterator != children.rend(); ++iterator) {
        AddChildAtIndex(*iterator, index);
    }
}


void ControlHierarchyFacet::RemoveChild(const std::shared_ptr<Control>& child) {
    InnerRemoveChild(child, true);
}


void ControlHierarchyFacet::InnerRemoveChild(
    const std::shared_ptr<Control>& child,
    bool set_parent_to_null) {

    auto current_parent = child->Parent();
    if (current_parent.get() != &control_) {
        //Not child
        return;
    }

    //Remove focus before removing child.
    auto window = Window();
    if (window) {
        auto focused_control = window->FocusedControl();
        if (focused_control) {
            if (child->IsSameOrAncestorOf(*focused_control)) {
                focused_control->SetIsFocused(false);
            }
        }
    }

    auto removed_iterator = std::find(children_.begin(), children_.end(), child);
    ZAF_EXPECT(removed_iterator != children_.end());

    auto removed_index = std::distance(children_.begin(), removed_iterator);
    children_.erase(removed_iterator);

    if (set_parent_to_null) {
        child->SetParent(nullptr);
    }

    //The child's rect may be changed while calling RequestLayout(), leading to a wrong repaint rect
    //while calling NeedRepaintRect(), so we preserve the original rect before calling 
    //RequestLayout().
    auto child_rect = child->Rect();

    control_.RequestLayout();
    control_.NeedRepaintRect(child_rect);

    control_.AutoResizeToPreferredSize();

    auto inspector_port = control_.GetInspectorPort();
    if (inspector_port) {
        inspector_port->ControlRemoveChild(control_.shared_from_this(), removed_index);
    }
}


void ControlHierarchyFacet::RemoveChildAtIndex(std::size_t index) {

    ZAF_EXPECT(index < children_.size());

    //We must use an local variable to maintain the life of the child for it will be accessed after 
    //deleted during remove.
    auto child = children_[index];
    RemoveChild(child);
}


void ControlHierarchyFacet::RemoveAllChildren() {

    std::shared_ptr<Control> focused_control;
    auto window = Window();
    if (window) {
        focused_control = window->FocusedControl();
    }

    for (const auto& each_child : children_) {

        //Remove focus before removing children.
        if (focused_control) {
            if (each_child->IsSameOrAncestorOf(*focused_control)) {
                focused_control->SetIsFocused(false);
            }
        }

        each_child->SetParent(nullptr);
    }

    children_.clear();
    control_.NeedRepaint();
}


std::shared_ptr<Control> ControlHierarchyFacet::FindChild(
    const std::wstring& name) const noexcept {

    for (const auto& each_child : children_) {
        if (each_child->Name() == name) {
            return each_child;
        }
    }

    for (const auto& each_child : children_) {
        auto found_child = each_child->FindChild(name);
        if (found_child) {
            return found_child;
        }
    }

    return {};
}


std::shared_ptr<Control> ControlHierarchyFacet::FindChildAtPosition(
    const Point& position) const noexcept {

    return InnerFindChildAtPosition(position, false);
}


std::shared_ptr<Control> ControlHierarchyFacet::FindChildAtPositionRecursively(
    const Point& position) const noexcept {

    return InnerFindChildAtPosition(position, true);
}


std::shared_ptr<Control> ControlHierarchyFacet::InnerFindChildAtPosition(
    const Point& position,
    bool recursively) const noexcept {

    auto content_rect = control_.ContentRectInSelf();

    auto position_in_content = position;
    position_in_content.x -= content_rect.position.x;
    position_in_content.y -= content_rect.position.y;

    content_rect.position.x = 0;
    content_rect.position.y = 0;

    for (auto iterator = children_.rbegin(); iterator != children_.rend(); ++iterator) {

        const auto& child = *iterator;

        if (!child->IsVisible()) {
            continue;
        }

        zaf::Rect child_rect = child->Rect();
        child_rect.Intersect(content_rect);

        if (!child_rect.Contains(position_in_content)) {
            continue;
        }

        if (!recursively) {
            return child;
        }

        auto recursive_child = child->hierarchy_facet_->InnerFindChildAtPosition(
            child->TranslateFromParent(position),
            true);

        if (recursive_child) {
            return recursive_child;
        }

        return child;
    }

    return nullptr;
}

}
