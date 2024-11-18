#pragma once

#include <zaf/control/expand_state.h>
#include <zaf/control/list_item.h>

namespace zaf {
namespace internal {
class TreeCore;
}

class TreeItemExpandButton;

class TreeItem : public ListItem {
public:
    std::size_t IndentLevel() const {
        return indent_level_;
    }

    ExpandState ExpandState() const;

    zaf::Rect ExpanderRect() const noexcept;
    zaf::Rect BodyRect() const noexcept;

protected:
    void Initialize() override;
    void Layout(const zaf::Rect& previous_rect) override;
    
private:
    friend class internal::TreeCore;

    void SetTreeCore(const std::shared_ptr<internal::TreeCore>& core) {
        tree_core_ = core;
    }

    void SetIndentLevel(std::size_t deep);
    void SetExpandState(zaf::ExpandState expand_state);

private:
    void ExpandButtonClick();

private:
    std::shared_ptr<TreeItemExpandButton> expand_button_;
    std::weak_ptr<internal::TreeCore> tree_core_;
    std::size_t indent_level_{};
};

}