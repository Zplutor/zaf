#pragma once

#include <zaf/base/non_copyable.h>
#include <zaf/graphic/point.h>

namespace zaf {
class Control;
class Window;
}

namespace zaf::internal {

class ControlHierarchyFacet : NonCopyableNonMovable {
public:
    explicit ControlHierarchyFacet(Control& control) noexcept;

    std::shared_ptr<zaf::Window> Window() const noexcept;
    std::shared_ptr<zaf::Window> DirectWindow() const noexcept;
    void SetDirectWindow(const std::shared_ptr<zaf::Window>& window) noexcept;

    std::shared_ptr<Control> Parent() const noexcept;

    bool IsParentOf(const Control& control) const noexcept;
    bool IsAncestorOf(const Control& control) const noexcept;
    bool IsSameOrAncestorOf(const Control& control) const noexcept;

    std::shared_ptr<Control> GetChildAtIndex(std::size_t index) const noexcept;
    const std::vector<std::shared_ptr<Control>>& Children() const noexcept;
    void SetChildren(const std::vector<std::shared_ptr<Control>>& children);

    void AddChild(const std::shared_ptr<Control>& child);
    void AddChildAtIndex(const std::shared_ptr<Control>& child, std::size_t index);
    void AddChildren(const std::vector<std::shared_ptr<Control>>& new_children);
    void AddChildrenAtIndex(
        const std::vector<std::shared_ptr<Control>>& new_children,
        std::size_t index);

    void RemoveChild(const std::shared_ptr<Control>& child);
    void RemoveChildAtIndex(std::size_t index);
    void RemoveAllChildren();

    std::shared_ptr<Control> FindChild(const std::wstring& name) const noexcept;
    std::shared_ptr<Control> FindChildAtPosition(const Point& position) const noexcept;
    std::shared_ptr<Control> FindChildAtPositionRecursively(const Point& position) const noexcept;
    std::shared_ptr<Control> InnerFindChildAtPosition(
        const Point& position,
        bool recursively) const noexcept;

private:
    void SetParent(const std::shared_ptr<Control>& parent) noexcept;
    void HandleParentChanged(
        const std::shared_ptr<Control>& old_parent, 
        const std::shared_ptr<Control>& new_parent) const;

    void InnerRemoveChild(const std::shared_ptr<Control>& child, bool will_add_to_new_parent);
    void InnerRemoveChildAtIndex(std::size_t index, bool will_add_to_new_parent);

private:
    Control& control_;

    std::weak_ptr<zaf::Window> window_;
    std::weak_ptr<Control> parent_;
    std::vector<std::shared_ptr<Control>> children_;
};

}
