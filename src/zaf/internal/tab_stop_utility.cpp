#include <zaf/internal/tab_stop_utility.h>
#include <algorithm>
#include <zaf/control/control.h>

namespace zaf {
namespace internal {

static const std::shared_ptr<Control> FindNextTabStopControlInChildren(
    const std::shared_ptr<Control>& parent,
    const std::shared_ptr<Control>& begin_child);

static const std::vector<std::shared_ptr<Control>> SortControlsByTabIndex(const std::vector<std::shared_ptr<Control>>& controls);

static std::vector<std::shared_ptr<Control>>::const_iterator FindBeginIterator(
    const std::shared_ptr<Control>& begin_control,
    const std::vector<std::shared_ptr<Control>>& controls);


const std::shared_ptr<Control> FindNextTabStopControl(const std::shared_ptr<Control>& control) {

    std::shared_ptr<Control> tab_stop_control = nullptr;

    auto parent = control;
    std::shared_ptr<Control> begin_child = nullptr;

    while (true) {

        tab_stop_control = FindNextTabStopControlInChildren(parent, begin_child);
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
    const std::shared_ptr<Control>& begin_child) {

    const auto& children = parent->GetChildren();
    if (children.empty()) {
        return nullptr;
    }

    auto sorted_children = SortControlsByTabIndex(children);

    auto begin_iterator = FindBeginIterator(begin_child, sorted_children);
    if (begin_iterator == sorted_children.end()) {
        return nullptr;
    }

    for (auto iterator = begin_iterator; iterator != sorted_children.end(); ++iterator) {

        const auto& child = *iterator;

        if (! child->IsEnabled()) {
            continue;
        }

        if (! child->CanTabStop()) {
            continue;
        }

        if (child->CanFocused()) {
            return child;
        }

        auto next_control = FindNextTabStopControlInChildren(child, nullptr);
        if (next_control != nullptr) {
            return next_control;
        }
    }

    return nullptr;
}


static const std::vector<std::shared_ptr<Control>> SortControlsByTabIndex(const std::vector<std::shared_ptr<Control>>& controls) {

    std::vector<std::shared_ptr<Control>> sorted_controls = controls;

    std::stable_sort(
        sorted_controls.begin(),
        sorted_controls.end(),
        [](const std::shared_ptr<Control>& control1, const std::shared_ptr<Control>& control2) {
        return control1->GetTabIndex() < control2->GetTabIndex();
    });

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