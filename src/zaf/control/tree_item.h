#pragma once

#include <zaf/control/expand_state.h>
#include <zaf/control/list_item.h>

namespace zaf {
namespace internal {
class TreeControlImplementation;
}

class TreeItemExpandButton;

class TreeItem : public ListItem {
public:
    std::size_t GetIndentDeep() const {
        return indent_deep_;
    }

    void SetIndentDeep(std::size_t deep);

    void SetExpandState(ExpandState expand_state);

protected:
    void Initialize() override;
    void Layout(const Rect& previous_rect) override;
    Rect GetTextRect() override;

    /**
     Get the rect of expand button, related to its content rect's coordinate system.
     */
    virtual Rect GetExpandButtonRect();

private:
    friend class internal::TreeControlImplementation;

    void SetTreeControlImplementation(
        const std::shared_ptr<internal::TreeControlImplementation>& implementation) {

        tree_control_implementation_ = implementation;
    }

private:
    void ExpandButtonClick();

private:
    std::shared_ptr<TreeItemExpandButton> expand_button_;
    std::weak_ptr<internal::TreeControlImplementation> tree_control_implementation_;
    std::size_t indent_deep_{};
};

}