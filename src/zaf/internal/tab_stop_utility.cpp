#include <zaf/internal/tab_stop_utility.h>
#include <algorithm>
#include <zaf/control/control.h>

namespace zaf {
namespace internal {

static const std::shared_ptr<Control> FindNextTabStopControlInChildren(
    const std::shared_ptr<Control>& parent,
    const std::shared_ptr<Control>& begin_child,
    bool backward);

static const std::vector<std::shared_ptr<Control>> SortControlsByTabIndex(
    const std::vector<std::shared_ptr<Control>>& controls, 
    bool reverse);

static std::vector<std::shared_ptr<Control>>::const_iterator FindBeginIterator(
    const std::shared_ptr<Control>& begin_control,
    const std::vector<std::shared_ptr<Control>>& controls);


const std::shared_ptr<Control> FindNextTabStopControl(const std::shared_ptr<Control>& control, bool backward) {

    std::shared_ptr<Control> tab_stop_control = nullptr;

    auto parent = control;
    std::shared_ptr<Control> begin_child = nullptr;

    while (true) {

        tab_stop_control = FindNextTabStopControlInChildren(parent, begin_child, backward);
        if (tab_stop_control != nullptr) {
            break;
        }

        auto next_parent = parent->GetParent();
        if (next_parent != nullptr) {   

            begin_child = parent;
            parent = next_parent;
        }
        else {

            if (begin_child != nullptr) {
                begin_child = nullptr;
            }
            else {
                break;
            }
        }
    }

    return tab_stop_control;
}


static const std::shared_ptr<Control> FindNextTabStopControlInChildren(
    const std::shared_ptr<Control>& parent,
    const std::shared_ptr<Control>& begin_child,
    bool backward) {

    const auto& children = parent->GetChildren();
    if (children.empty()) {
        return nullptr;
    }

    auto sorted_children = SortControlsByTabIndex(children, backward);

    auto begin_iterator = FindBeginIterator(begin_child, sorted_children);
    if (begin_iterator == sorted_children.end()) {
        return nullptr;
    }

    for (auto iterator = begin_iterator; iterator != sorted_children.end(); ++iterator) {

        const auto& child = *iterator;

        if (! child->IsVisible()) {
            continue;
        }

        if (! child->IsEnabled()) {
            continue;
        }

        if (! child->CanTabStop()) {
            continue;
        }

        if (child->CanFocused()) {
            return child;
        }

        auto next_control = FindNextTabStopControlInChildren(child, nullptr, backward);
        if (next_control != nullptr) {
            return next_control;
        }
    }

    return nullptr;
}


static const std::vector<std::shared_ptr<Control>> SortControlsByTabIndex(
    const std::vector<std::shared_ptr<Control>>& controls, 
    bool reverse) {

    std::vector<std::shared_ptr<Control>> sorted_controls = controls;

    std::stable_sort(
        sorted_controls.begin(),
        sorted_controls.end(),
        [](const std::shared_ptr<Control>& control1, const std::shared_ptr<Control>& control2) {

        auto tab_index1 = control1->GetTabIndex();
        auto tab_index2 = control2->GetTabIndex();

        if (tab_index1 && !tab_index2) {
            return true;
        }

        if (!tab_index1 && tab_index2) {
            return false;
        }

        return *tab_index1 < *tab_index2;
    });

    if (reverse) {
        std::reverse(sorted_controls.begin(), sorted_controls.end());
    }

    return sorted_controls;
}


static std::vector<std::shared_ptr<Control>>::const_iterator FindBeginIterator(
    const std::shared_ptr<Control>& begin_control,
    const std::vector<std::shared_ptr<Control>>& controls) {

    if (begin_control == nullptr) {
        return controls.begin();
    }

    auto iterator = std::find(controls.begin(), controls.end(), begin_control);
    if (iterator == controls.end()) {
        return iterator;
    }

    ++iterator;
    return iterator;
}

}
}