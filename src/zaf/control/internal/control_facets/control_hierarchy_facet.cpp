#include <zaf/control/internal/control_facets/control_hierarchy_facet.h>
#include <zaf/application.h>
#include <zaf/base/error/invalid_operation_error.h>
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


void ControlHierarchyFacet::SetParent(const std::shared_ptr<Control>& parent) noexcept {
    parent_ = parent;
}


void ControlHierarchyFacet::HandleParentChanged(
    const std::shared_ptr<Control>& old_parent,
    const std::shared_ptr<Control>& new_parent) const {

    // No need further handling if parent is not changed.
    if (old_parent == new_parent) {
        return;
    }

    std::shared_ptr<zaf::Window> old_window;
    if (old_parent) {
        old_window = old_parent->Window();
    }

    std::shared_ptr<zaf::Window> new_window;
    if (new_parent) {
        new_window = new_parent->Window();
    }

    if (old_window && (old_window != new_window)) {
        control_.ReleaseRendererResources();
    }

    if (new_parent) {
        control_.NeedUpdateStyle();
    }

    ParentChangedInfo parent_changed_info{ control_.shared_from_this(), old_parent };
    control_.OnParentChanged(parent_changed_info);

    if (old_window != new_window) {

        control_.RaiseWindowChangedEvent(old_window);

        if (GetDPIFromWindow(old_window) != GetDPIFromWindow(new_window)) {
            control_.RaiseDPIChangedEvent();
        }
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

    // Prevent circular reference.
    if (child->IsSameOrAncestorOf(control_)) {
        throw InvalidOperationError{ ZAF_SOURCE_LOCATION() };
    }

    // Not allow to add a child that is a window's root control.
    if (child->hierarchy_facet_->DirectWindow()) {
        throw InvalidOperationError{ ZAF_SOURCE_LOCATION() };
    }

    auto child_old_parent = child->Parent();
    if (child_old_parent) {
        // If the child is already added to this control, do nothing.
        if (child_old_parent.get() == &control_) {
            return;
        }
        // Remove the child from its previous parent.
        child_old_parent->hierarchy_facet_->InnerRemoveChild(child, true);
    }

    // The following code must be atomic. If exception is thrown, neither children_ nor the child's
    // parent will be changed.
    auto revised_index = std::min(index, children_.size());
    auto insert_iterator = std::next(children_.begin(), revised_index);
    children_.insert(insert_iterator, child);

    auto child_new_parent = control_.shared_from_this();
    child->hierarchy_facet_->SetParent(child_new_parent);
    // Atomic codes end. It is safe to throw exception after here.

    child->hierarchy_facet_->HandleParentChanged(child_old_parent, child_new_parent);

    control_.RequestLayout();
    control_.AutoResizeToPreferredSize();
    control_.NeedRepaintRect(child->Rect());

    auto inspector_port = control_.GetInspectorPort();
    if (inspector_port) {
        inspector_port->ControlAddChild(child_new_parent, index);
    }
}


void ControlHierarchyFacet::AddChildren(
    const std::vector<std::shared_ptr<Control>>& new_children) {

    AddChildrenAtIndex(new_children, children_.size());
}


void ControlHierarchyFacet::AddChildrenAtIndex(
    const std::vector<std::shared_ptr<Control>>& new_children,
    std::size_t index) {

    if (new_children.empty()) {
        return;
    }

    // Reserve enough space for new children, preventing the bad alloc exception.
    children_.reserve(children_.size() + new_children.size());

    auto update_guard = control_.BeginUpdate();

    std::size_t next_index = index;
    for (const auto& each_child : new_children) {
        AddChildAtIndex(each_child, next_index);
        ++next_index;
    }
}


void ControlHierarchyFacet::RemoveChild(const std::shared_ptr<Control>& child) {
    ZAF_EXPECT(child);
    InnerRemoveChild(child, false);
}


void ControlHierarchyFacet::RemoveChildAtIndex(std::size_t index) {
    InnerRemoveChildAtIndex(index, false);
}


void ControlHierarchyFacet::InnerRemoveChild(
    const std::shared_ptr<Control>& child,
    bool will_add_to_new_parent) {

    auto iterator = std::find(children_.begin(), children_.end(), child);
    if (iterator == children_.end()) {
        return;
    }

    auto index = std::distance(children_.begin(), iterator);
    InnerRemoveChildAtIndex(index, will_add_to_new_parent);
}


void ControlHierarchyFacet::InnerRemoveChildAtIndex(
    std::size_t index,
    bool will_add_to_new_parent) {

    if (index >= children_.size()) {
        return;
    }

    // The child should be stored in a local variable to maintain its life, because it will be 
    // accessed after being removed from children_.
    auto child = children_[index];

    // If the focused control is the child or its descendant, remove the focus before removing the 
    // child.
    auto window = Window();
    if (window) {
        auto focused_control = window->FocusedControl();
        if (focused_control) {
            if (child->IsSameOrAncestorOf(*focused_control)) {
                focused_control->SetIsFocused(false);
            }
        }
    }

    auto remove_iterator = std::next(children_.begin(), index);
    children_.erase(remove_iterator);

    // It MUST NOT call control_.shared_from_this() here to get the old parent, because this method
    // may be called while the parent is being destructed.
    auto child_old_parent = child->Parent();
    child->hierarchy_facet_->SetParent(nullptr);

    if (!will_add_to_new_parent) {
        child->hierarchy_facet_->HandleParentChanged(child_old_parent, nullptr);
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
        inspector_port->ControlRemoveChild(control_.shared_from_this(), index);
    }
}


void ControlHierarchyFacet::RemoveAllChildren() {

    // Make a copy of children_ to avoid the problem of modifying children_ while iterating it.
    auto all_children = children_;

    auto update_guard = control_.BeginUpdate();
    for (const auto& each_child : all_children) {
        RemoveChild(each_child);
    }
}


std::shared_ptr<Control> ControlHierarchyFacet::FindChild(std::wstring_view name) const noexcept {
    for (const auto& each_child : children_) {
        if (each_child->Name() == name) {
            return each_child;
        }
    }
    return nullptr;
}


std::shared_ptr<Control> ControlHierarchyFacet::FindDescendant(
    std::wstring_view name) const noexcept {

    auto found_child = FindChild(name);
    if (found_child) {
        return found_child;
    }

    for (const auto& each_child : children_) {
        found_child = each_child->FindDescendant(name);
        if (found_child) {
            return found_child;
        }
    }

    return nullptr;
}


std::shared_ptr<Control> ControlHierarchyFacet::FindChildAtPosition(
    const Point& position) const noexcept {

    return InnerFindChildAtPosition(position, false);
}


std::shared_ptr<Control> ControlHierarchyFacet::FindDescendantAtPosition(
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
